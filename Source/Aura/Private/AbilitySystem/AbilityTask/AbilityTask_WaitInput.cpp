// Copyright Hung


#include "AbilitySystem/AbilityTask/AbilityTask_WaitInput.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"

UAbilityTask_WaitSpecInputPressed* UAbilityTask_WaitSpecInputPressed::WaitSpecInputPressed(
	UAuraGameplayAbility* OwningAbility)
{
	return NewAbilityTask<UAbilityTask_WaitSpecInputPressed>(OwningAbility);
}

void UAbilityTask_WaitSpecInputPressed::Activate()
{
	StartTime = GetWorld()->GetTimeSeconds();

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC || !Ability) return;

	DelegateHandle = ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputPressed,
		Ability->GetCurrentAbilitySpecHandle(), GetActivationPredictionKey())
		.AddUObject(this, &UAbilityTask_WaitSpecInputPressed::OnPressedCallback);
	if (IsForRemoteClient() &&
		!ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::InputPressed,
			Ability->GetCurrentAbilitySpecHandle(), GetActivationPredictionKey()))
	{
		SetWaitingOnRemotePlayerData();
	}
}

void UAbilityTask_WaitSpecInputPressed::OnPressedCallback()
{
	const float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime; // Time since ability activated (Input Started)

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Ability || !ASC) return;

	const FGameplayAbilitySpecHandle SpecHandle = Ability->GetCurrentAbilitySpecHandle();
	const FPredictionKey& PredictionKey = Ability->GetCurrentActivationInfo().GetActivationPredictionKey();

	ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputPressed, SpecHandle, PredictionKey).Remove(
		DelegateHandle);

	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());

	if (IsPredictingClient())
	{	// Tell the server about this
		ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, SpecHandle,
			PredictionKey, ASC->ScopedPredictionKey);
	}
	else ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, SpecHandle, PredictionKey);

	// We are done. Kill us so we don't keep getting broadcast messages
	if (ShouldBroadcastAbilityTaskDelegates()) OnPress.Broadcast(ElapsedTime);

	EndTask();
}

void UAbilityTask_WaitSpecInputPressed::OnDestroy(bool bInOwnerFinished)
{	// Called in EndTask or when EndAbility
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Ability || !ASC) return;
	ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(),
		GetActivationPredictionKey()).Remove(DelegateHandle);
	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());
	if (IsPredictingClient())
	{	// Tell the server about this
		ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(),
		GetActivationPredictionKey(), ASC->ScopedPredictionKey);
	}
	else ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(),
		GetActivationPredictionKey());
	Super::OnDestroy(bInOwnerFinished);
}


/*
 * RELEASED =================================================================================================
 */
UAbilityTask_WaitSpecInputReleased* UAbilityTask_WaitSpecInputReleased::WaitSpecInputReleased(
	UAuraGameplayAbility* OwningAbility)
{
	return NewAbilityTask<UAbilityTask_WaitSpecInputReleased>(OwningAbility);
}

void UAbilityTask_WaitSpecInputReleased::Activate()
{
	StartTime = GetWorld()->GetTimeSeconds();

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC || !Ability) return;

	DelegateHandle = ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputReleased,
		Ability->GetCurrentAbilitySpecHandle(), GetActivationPredictionKey())
		.AddUObject(this, &UAbilityTask_WaitSpecInputReleased::OnReleasedCallback);
	if (IsForRemoteClient() &&
		!ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::InputReleased,
			Ability->GetCurrentAbilitySpecHandle(), GetActivationPredictionKey()))
	{
		SetWaitingOnRemotePlayerData();
	}
}

void UAbilityTask_WaitSpecInputReleased::OnReleasedCallback()
{
	const float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Ability || !ASC) return;

	const FGameplayAbilitySpecHandle SpecHandle = Ability->GetCurrentAbilitySpecHandle();
	const FPredictionKey& PredictionKey = Ability->GetCurrentActivationInfo().GetActivationPredictionKey();

	ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputReleased, SpecHandle, PredictionKey).Remove(
		DelegateHandle);

	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());

	if (IsPredictingClient())
	{	// Tell the server about this
		ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, SpecHandle,
			PredictionKey, ASC->ScopedPredictionKey);
	}
	else ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, SpecHandle, PredictionKey);

	// We are done. Kill us so we don't keep getting broadcast messages
	if (ShouldBroadcastAbilityTaskDelegates()) OnReleased.Broadcast(ElapsedTime);

	EndTask();
}

void UAbilityTask_WaitSpecInputReleased::OnDestroy(bool bInOwnerFinished)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Ability || !ASC) return;
	ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputReleased, GetAbilitySpecHandle(),
		GetActivationPredictionKey()).Remove(DelegateHandle);
	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());
	if (IsPredictingClient())
	{	// Tell the server about this
		ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, GetAbilitySpecHandle(),
		GetActivationPredictionKey(), ASC->ScopedPredictionKey);
	}
	else ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, GetAbilitySpecHandle(),
		GetActivationPredictionKey());
	Super::OnDestroy(bInOwnerFinished);
}
