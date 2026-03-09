// Copyright Hung


#include "AbilitySystem/AsyncTask/Async_CooldownChange.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"

UAsync_CooldownChange* UAsync_CooldownChange::WaitForCooldownChange(UAuraAbilitySystemComponent* InASC,
	const FGameplayTagContainer& InCooldownTags, const bool InUseServerCooldown)
{
	if (!IsValid(InASC) || InCooldownTags.IsEmpty()) return nullptr;
	UAsync_CooldownChange* AsyncAction = NewObject<UAsync_CooldownChange>();
	AsyncAction->ASC = InASC;
	AsyncAction->CooldownTags = InCooldownTags;
	AsyncAction->bUseServerCooldown = InUseServerCooldown;
	// GE_Cooldown Applied. Have access to the GameplayEffectSpec that applied it.
	// Recommended because From GameplayEffectSpec you can determine if the Cooldown GE is the locally predicted one or the Server's correcting one.
	AsyncAction->ASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(AsyncAction, &UAsync_CooldownChange::OnActiveEffectAdded);

	AsyncAction->ASC->OnAnyGameplayEffectRemovedDelegate().AddWeakLambda(AsyncAction,
	[AsyncAction](const FActiveGameplayEffect& ActiveEffect)
	{
		FGameplayTagContainer EffectTags; ActiveEffect.Spec.GetAllGrantedTags(EffectTags); //.GetAllAssetTags(EffectTags);
		if (EffectTags.HasAnyExact(AsyncAction->CooldownTags)) AsyncAction->InitWaitCooldown();
	});

	/*for (const FGameplayTag& Tag : AsyncAction->CooldownTags = InCooldownTags)
	{	/* Tag Changed. Recommended because when the Server's Cooldown GE comes in, it will remove our locally predicted CD,
		 * broadcasting OnAnyGameplayEffectRemovedDelegate() despite just receiving Server correction and still on CD.
		 * RegisterGameplayTagEvent will not fire during the removal of the predicted CD and the application of Server's corrected CD.#1#
		InASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(AsyncAction /*static func, don't use 'this'#1# , &UAsync_CooldownChange::CooldownTagChanged);
	}*/

	return AsyncAction;
}

void UAsync_CooldownChange::EndTask()
{
	if (IsValid(ASC))
	{
		ASC->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
		/*for (const FGameplayTag& Tag : CooldownTags)
		{
			ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
		}*/
		ASC->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
	}

	SetReadyToDestroy();
	MarkAsGarbage();
}

// For when input is changed during cooldown, Task should be re-created before calling this
void UAsync_CooldownChange::InitWaitCooldown()
{
	CheckCooldown();
	if (CooldownTime > UE_KINDA_SMALL_NUMBER) CooldownChanged.Broadcast(CooldownTime, CooldownDuration); // On CD
	else CooldownEnd.Broadcast(0.f, 0.f);
}

void UAsync_CooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& SpecApplied, const FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer EffectTags; SpecApplied.GetAllGrantedTags(EffectTags); //SpecApplied.GetAllAssetTags(EffectTags);
	if (!EffectTags.HasAnyExact(CooldownTags)) return;

	CheckCooldown();

	if (ASC->GetOwnerRole() == ROLE_Authority) CooldownChanged.Broadcast(CooldownTime, CooldownDuration); /*Is Server*/
	else // Client
	{	// _NotReplicated() will return null for Context received from server, != nullptr for local prediction
		const bool bIsFromServer = SpecApplied.GetContext().GetAbilityInstance_NotReplicated() == nullptr;
		if ((!bUseServerCooldown && !bIsFromServer) || /*Use Local Predicted*/
			(bUseServerCooldown && bIsFromServer)) /*Use Server and just Received Server's CD*/
		{
			CooldownChanged.Broadcast(CooldownTime, CooldownDuration);
		}
		else if (bUseServerCooldown && !bIsFromServer)
		{	/* using Server's cooldown but is "predicted". Gray-out ability Widget until Server's CD comes in*/
			CooldownChanged.Broadcast(-1.f, -1.f);
		}
	}
}

void UAsync_CooldownChange::CheckCooldown() // Ability's CD can be affected by multiple tags/effects
{	// UGameplayAbility::GetCooldownTimeRemainingAndDuration() returns largest TimeRemaining
	const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
	CooldownTime = CooldownDuration = 0.f;
	for (auto [TimeRemaining, Duration] : ASC->GetActiveEffectsTimeRemainingAndDuration(Query))
	{
		if (TimeRemaining > CooldownTime)
		{
			CooldownTime = TimeRemaining;
			CooldownDuration = Duration;
		}
	}
}
