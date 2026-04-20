// Copyright Hung


#include "AbilitySystem/AbilityTask/AT_WaitGenericEvent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"

UAT_WaitGenericEvent* UAT_WaitGenericEvent::WaitGenericReplicatedEvent(class UAuraGameplayAbility* OwningAbility,
	const EAbilityGenericReplicatedEvent::Type Event)
{
	UAT_WaitGenericEvent* Task = NewAbilityTask<UAT_WaitGenericEvent>(OwningAbility);
	Task->Event = Event;
	return Task;
}

void UAT_WaitGenericEvent::OnDestroy(bool bInOwnerFinished)
{
	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		ASC->AbilityReplicatedEventDelegate(Event, GetAbilitySpecHandle(), GetActivationPredictionKey()).RemoveAll(this);
	}
	Super::OnDestroy(bInOwnerFinished);
}

void UAT_WaitGenericEvent::Activate()
{
	StartTime = GetWorld()->GetTimeSeconds();

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC || !Ability) return;

	const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
	const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
	ASC->AbilityReplicatedEventDelegate(Event, SpecHandle, ActivationPredictionKey).AddUObject(this,
		&UAT_WaitGenericEvent::OnCallback);
	if (IsForRemoteClient())
	{
		if (!ASC->CallReplicatedEventDelegateIfSet(Event, SpecHandle, ActivationPredictionKey))
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UAT_WaitGenericEvent::OnCallback()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Ability || !ASC) return;

	const FGameplayAbilitySpecHandle SpecHandle = Ability->GetCurrentAbilitySpecHandle();
	const FPredictionKey& PredictionKey = Ability->GetCurrentActivationInfo().GetActivationPredictionKey();

	// Done. Remove all delegates in this Task
	ASC->AbilityReplicatedEventDelegate(Event, SpecHandle, PredictionKey).RemoveAll(this);

	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());

	if (IsPredictingClient())
	{	// Tell the server about this
		ASC->ServerSetReplicatedEvent(Event, SpecHandle, PredictionKey,
			ASC->ScopedPredictionKey);
	}
	else ASC->ConsumeGenericReplicatedEvent(Event, SpecHandle, PredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		const float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime; // Time since task activated
		OnReceived.Broadcast(ElapsedTime);
	}
	EndTask(); // Delegate already removed, Task now do nothing
}
