// Copyright Hung


#include "AbilitySystem/AsyncTask/Async_CooldownChange.h"

#include "AbilitySystemComponent.h"

UAsync_CooldownChange* UAsync_CooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayTagContainer& InCooldownTags, const bool InUseServerCooldown)
{
	UAsync_CooldownChange* WaitCooldownChange = NewObject<UAsync_CooldownChange>();
	WaitCooldownChange->ASC = AbilitySystemComponent;
	WaitCooldownChange->CooldownTags = InCooldownTags;
	WaitCooldownChange->bUseServerCooldown = InUseServerCooldown;
	if (!IsValid(AbilitySystemComponent) || InCooldownTags.IsEmpty())
	{
		WaitCooldownChange->EndTask();
		return nullptr;
	}

	// GE_Cooldown Applied.
	// Recommended because you also have access to the GameplayEffectSpec that applied it.
	// From this you can determine if the Cooldown GE is the locally predicted one or the Server's correcting one.
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &UAsync_CooldownChange::OnActiveEffectAdded);

	// ReduceCooldownByTag() will trigger this. but not RegisterGameplayTagEvent()
	AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(WaitCooldownChange, &UAsync_CooldownChange::OnGameplayEffectRemoved);

	// CooldownTag Removed.
	// Recommended because when the Server's corrected Cooldown GE comes in, it will remove our locally predicted one
	// causing the OnAnyGameplayEffectRemovedDelegate() to fire even though we're still on cooldown.
	// The Cooldown Tag will not change during the removal of the predicted Cooldown GE and the application of the Server's corrected Cooldown GE.
	for (const FGameplayTag& Tag : InCooldownTags)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(WaitCooldownChange /* static function, 'this' means nothing */, &UAsync_CooldownChange::CooldownTagChanged);
	}

	return WaitCooldownChange;
}

void UAsync_CooldownChange::EndTask()
{
	if (!IsValid(ASC)) return;
	for (const FGameplayTag& Tag : CooldownTags)
	{
		ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	}
	ASC->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
	ASC->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);

	SetReadyToDestroy();
	MarkAsGarbage();
}

// For when input is changed during cooldown
void UAsync_CooldownChange::BroadcastInitialCooldown() const
{
	const FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
	TArray<TPair<float, float>> TimeAndDurations = ASC->GetActiveEffectsTimeRemainingAndDuration(GameplayEffectQuery);
	for (const TPair<float, float>& Pair : TimeAndDurations)
	{
		CooldownChanged.Broadcast(Pair.Key, Pair.Value); break;
	}
}

void UAsync_CooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,
                                                const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	/*FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);*/
	FGameplayTagContainer GrantedTags; SpecApplied.GetAllGrantedTags(GrantedTags);

	if (GrantedTags.HasAnyExact(CooldownTags))
	{
		// CooldownTag.GetSingleTagContainer();
		const FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
		if (CooldownTime > UE_KINDA_SMALL_NUMBER) // still on cooldown
		{
			const TArray<float> TimesRemaining = TargetASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
			CooldownTime = FMath::Max(TimesRemaining); // if empty, returns 0;
		}
		else // New Cooldown Session
		{
			for (const TPair Pair : TargetASC->GetActiveEffectsTimeRemainingAndDuration(GameplayEffectQuery))
			{
				if (CooldownTime < Pair.Key)
				{
					CooldownTime = Pair.Key;
					CooldownDuration = Pair.Value;
				}
			}
		}

		if (TargetASC->GetOwnerRole() == ROLE_Authority /*Player is Server*/
			/*Client's Predicted cooldown*/
			|| !bUseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated()
			/*Client using Server's corrective cooldown*/
			|| bUseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated() == nullptr
			)
		{
			CooldownChanged.Broadcast(CooldownTime, CooldownDuration);
		}
		else if (bUseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated())
		{
			/* using Server's cooldown but is "predicted" cooldown
			 * Gray out ability Widget until Server broadcast OnActiveGameplayEffectAddedDelegateToSelf */
			CooldownChanged.Broadcast(-1.f, -1.f);
		}
	}
}

void UAsync_CooldownChange::OnGameplayEffectRemoved(const FActiveGameplayEffect& ActiveEffect)
{
	FGameplayTagContainer GrantedTags; ActiveEffect.Spec.GetAllGrantedTags(GrantedTags);
	if (GrantedTags.HasAnyExact(CooldownTags))
	{	// Remaining Cooldown before Replaced
		CooldownTime = ActiveEffect.GetTimeRemaining(ASC->GetWorld()->GetTimeSeconds()); /*AsyncAction has NO World*/
	}
}

void UAsync_CooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, const int32 NewCount)
{
	if (InCooldownTag.MatchesAnyExact(CooldownTags) && NewCount == 0)
	{
		CooldownTime = 0.f;
		CooldownEnd.Broadcast();
	}
}
