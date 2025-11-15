// Copyright Hung


#include "AbilitySystem/AsyncTask/Async_CooldownChange.h"

#include "AbilitySystemComponent.h"

UAsync_CooldownChange* UAsync_CooldownChange::WaitForCooldownChange(UAbilitySystemComponent* InASC,
	const FGameplayTagContainer& InCooldownTags, const bool InUseServerCooldown)
{
	UAsync_CooldownChange* WaitCDChange = NewObject<UAsync_CooldownChange>();
	WaitCDChange->ASC = InASC;
	WaitCDChange->CooldownTags = InCooldownTags;
	WaitCDChange->bUseServerCooldown = InUseServerCooldown;
	if (!IsValid(InASC) || InCooldownTags.IsEmpty())
	{
		WaitCDChange->EndTask();
		return nullptr;
	}
	
	// GE_Cooldown Applied.
	// Recommended because you also have access to the GameplayEffectSpec that applied it.
	// From this you can determine if the Cooldown GE is the locally predicted one or the Server's correcting one.
	InASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCDChange, &UAsync_CooldownChange::OnActiveEffectAdded);
	
	// ReduceCooldownByTag()->RemoveActiveGameplayEffect() will trigger this. but not RegisterGameplayTagEvent()
	// InASC->OnAnyGameplayEffectRemovedDelegate().AddUObject(WaitCDChange, &UAsync_CooldownChange::OnGameplayEffectRemoved);
	
	/*// CooldownTag Removed. //Using OnAnyGameplayEffectRemovedDelegate() instead
	// Recommended because when the Server's corrected Cooldown GE comes in, it will remove our locally predicted one
	// causing the OnAnyGameplayEffectRemovedDelegate() to fire even though we're still on cooldown.
	// The Cooldown Tag will not change during the removal of the predicted Cooldown GE and the application of the Server's corrected Cooldown GE.
	for (const FGameplayTag& Tag : InCooldownTags)
	{
		InASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(WaitCDChange/* static func, don't use 'this' #1#, &UAsync_CooldownChange::CooldownTagChanged);
	}*/
	
	return WaitCDChange;
}

void UAsync_CooldownChange::EndTask()
{
	if (!IsValid(ASC)) return;
	/*for (const FGameplayTag& Tag : CooldownTags) // Using OnAnyGameplayEffectRemovedDelegate() instead
	{
		ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	}*/
	ASC->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
	// ASC->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
	if (FOnActiveGameplayEffectRemoved_Info* DelPtr = ASC->OnGameplayEffectRemoved_InfoDelegate(EffectHandle))
	{
		DelPtr->Remove(OnEffectRemovedDelegate);
	}
	
	SetReadyToDestroy();
	MarkAsGarbage();
}

// For when input is changed during cooldown
void UAsync_CooldownChange::BroadcastInitialCooldown() const
{
	const FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
	for (auto [TimeRemain, Duration] : ASC->GetActiveEffectsTimeRemainingAndDuration(GameplayEffectQuery))
	{
		CooldownChanged.Broadcast(TimeRemain, Duration); break;
	}
}

void UAsync_CooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& SpecApplied, const FActiveGameplayEffectHandle ActiveEffectHandle)
{
	/*FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);*/
	FGameplayTagContainer GrantedTags; SpecApplied.GetAllGrantedTags(GrantedTags);
	
	if (!GrantedTags.HasAnyExact(CooldownTags)) return; // CooldownTag.GetSingleTagContainer();
	const FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
	if (CooldownTime > UE_KINDA_SMALL_NUMBER) // still on cooldown
	{
		const TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
		CooldownTime = FMath::Max(TimesRemaining); // if empty, returns 0;
	}
	else // New Cooldown Session
	{
		for (auto [Remain, Duration] : ASC->GetActiveEffectsTimeRemainingAndDuration(GameplayEffectQuery))
		{
			if (CooldownTime < Remain)
			{
				CooldownTime = Remain;
				CooldownDuration = Duration;
			}
		}
	}
	
	if (ASC->GetOwnerRole() == ROLE_Authority /*Player is Server*/
		/*Client's Predicted cooldown*/
		|| !bUseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated()
		/*Client using Server's corrective cooldown*/
		|| bUseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated() == nullptr
		)
	{
		CooldownChanged.Broadcast(CooldownTime, CooldownDuration);
	}
	else if (bUseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated())
	{	/* using Server's cooldown but is "predicted" cooldown
		 * Gray out ability Widget until Server broadcast OnActiveGameplayEffectAddedDelegateToSelf */
		CooldownChanged.Broadcast(-1.f, -1.f);
	}
	
	if (FOnActiveGameplayEffectRemoved_Info* DelPtr = ASC->OnGameplayEffectRemoved_InfoDelegate(EffectHandle))
	{
		DelPtr->Remove(OnEffectRemovedDelegate);
	}
	EffectHandle = ActiveEffectHandle;
	OnEffectRemovedDelegate = ASC->OnGameplayEffectRemoved_InfoDelegate(EffectHandle)
		->AddUObject(this, &UAsync_CooldownChange::OnEffectRemoved);
}

void UAsync_CooldownChange::OnGameplayEffectRemoved(const FActiveGameplayEffect& ActiveEffect)
{
	const FGameplayTagContainer DynamicGranted = ActiveEffect.Spec.DynamicGrantedTags;
	// FGameplayTagContainer GrantedTags; ActiveEffect.Spec.GetAllGrantedTags(GrantedTags);
	if (DynamicGranted.HasAnyExact(CooldownTags))
	{	// Remaining Cooldown before Replaced
		CooldownTime = ActiveEffect.GetTimeRemaining(ASC->GetWorld()->GetTimeSeconds()); /*AsyncAction has NO World*/
		if (CooldownTime < UE_KINDA_SMALL_NUMBER) CooldownEnd.Broadcast();
	}
}

//TODO: this is not being used, remove this
/*void UAsync_CooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, const int32 NewCount)
{
	if (InCooldownTag.MatchesAnyExact(CooldownTags) && NewCount == 0)
	{
		CooldownTime = 0.f;
		CooldownEnd.Broadcast();
	}
}*/

void UAsync_CooldownChange::OnEffectRemoved(const FGameplayEffectRemovalInfo& Info)
{
	if (Info.ActiveEffect->Spec.DynamicGrantedTags.HasAnyExact(CooldownTags))
	{	// Remaining Cooldown if being Replaced
		CooldownTime = Info.ActiveEffect->GetTimeRemaining(ASC->GetWorld()->GetTimeSeconds()); /*AsyncAction has NO World*/
		if (CooldownTime < UE_KINDA_SMALL_NUMBER) CooldownEnd.Broadcast();
	}
}
