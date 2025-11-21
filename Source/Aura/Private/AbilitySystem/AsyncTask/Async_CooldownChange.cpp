// Copyright Hung


#include "AbilitySystem/AsyncTask/Async_CooldownChange.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"

UAsync_CooldownChange* UAsync_CooldownChange::WaitForCooldownChange(UAbilitySystemComponent* InASC,
                                                                    const FGameplayTagContainer& InCooldownTags, const bool InUseServerCooldown)
{
	if (!IsValid(InASC) || InCooldownTags.IsEmpty()) return nullptr;
	UAsync_CooldownChange* WaitCDChange = NewObject<UAsync_CooldownChange>();
	WaitCDChange->ASC = InASC;
	WaitCDChange->CooldownTags = InCooldownTags;
	WaitCDChange->bUseServerCooldown = InUseServerCooldown;

	// GE_Cooldown Applied. Have access to the GameplayEffectSpec that applied it.
	// From this you can determine if the Cooldown GE is the locally predicted one or the Server's correcting one.
	InASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCDChange, &UAsync_CooldownChange::OnActiveEffectAdded);

	/* Tag Changed. Recommended because when the Server's Cooldown GE comes in, it will remove our locally predicted one,
	 * firing OnAnyGameplayEffectRemovedDelegate() even though we've just received Server correction and still on CD.
	 * The Cooldown Tag will not change during the removal of the predicted Cooldown GE and the application of the Server's corrected Cooldown GE.*/
	for (const FGameplayTag& Tag : InCooldownTags)
	{
		InASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(WaitCDChange/* static func, don't use 'this' */, &UAsync_CooldownChange::CooldownTagChanged);
	}

	return WaitCDChange;
}

void UAsync_CooldownChange::EndTask()
{
	if (IsValid(ASC))
	{
		ASC->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
		for (const FGameplayTag& Tag : CooldownTags)
		{
			ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
		}
	}

	SetReadyToDestroy();
	MarkAsGarbage();
}

// For when input is changed during cooldown
void UAsync_CooldownChange::InitWaitCooldown()
{
	const FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
	for (const FActiveGameplayEffectHandle ActiveHandle : ASC->GetActiveEffects(GameplayEffectQuery))
	{	// Set Cooldown
		CheckCooldown(ActiveHandle);
	}
	if (CooldownTime > UE_KINDA_SMALL_NUMBER)
	{	// if is on cooldown
		CooldownChanged.Broadcast(CooldownTime, CooldownDuration);
	}
}

void UAsync_CooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& SpecApplied, const FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer EffectTags; SpecApplied.GetAllGrantedTags(EffectTags); //SpecApplied.GetAllAssetTags(EffectTags);
	if (!EffectTags.HasAnyExact(CooldownTags)) return; // CooldownTag.GetSingleTagContainer();

	CheckCooldown(ActiveEffectHandle);

	if (ASC->GetOwnerRole() == ROLE_Authority) CooldownChanged.Broadcast(CooldownTime, CooldownDuration); /*Is Server*/
	else // Client
	{	// _NotReplicated() will return null for Context received from server, != nullptr for local prediction
		const bool bIsFromServer = SpecApplied.GetContext().GetAbilityInstance_NotReplicated() == nullptr;
		if ((!bUseServerCooldown && !bIsFromServer) || /*Local Predicted Cooldown*/
			(bUseServerCooldown && bIsFromServer)) /*Receive Server's CD*/
		{
			CooldownChanged.Broadcast(CooldownTime, CooldownDuration);
		}
		else if (bUseServerCooldown && !bIsFromServer)
		{	/* using Server's cooldown but is "predicted". Gray-out ability Widget until Server's CD comes in*/
			CooldownChanged.Broadcast(-1.f, -1.f);
		}
	}
}

void UAsync_CooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (InCooldownTag.MatchesAnyExact(CooldownTags) && NewCount == 0)
	{
		CooldownTime = 0.f;
		CooldownEnd.Broadcast();
	}
}

void UAsync_CooldownChange::CheckCooldown(const FActiveGameplayEffectHandle ActiveHandle)
{
	const FActiveGameplayEffect* GE = ASC->GetActiveGameplayEffect(ActiveHandle);
	CooldownTime = GE->GetTimeRemaining(ASC->GetWorld()->GetTimeSeconds());
	if (const float* Duration = GE->Spec.SetByCallerTagMagnitudes.Find(AuraGameplayTags::Ability_Cooldown_Duration))
	{
		CooldownDuration = *Duration;
	}
	else CooldownDuration = GE->GetDuration();
}
