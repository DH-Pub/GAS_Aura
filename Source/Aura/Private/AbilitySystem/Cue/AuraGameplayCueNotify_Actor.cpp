// Copyright Hung


#include "AbilitySystem/Cue/AuraGameplayCueNotify_Actor.h"

AAuraGameplayCueNotify_Actor::AAuraGameplayCueNotify_Actor()
{
	bAutoDestroyOnRemove = true;
}

void AAuraGameplayCueNotify_Actor::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType,
	const FGameplayCueParameters& Parameters)
{	// Super::HandleGameplayCue(MyTarget, EventType, Parameters);
	switch (EventType)
	{
	case EGameplayCueEvent::WhileActive:
		if (PrimaryActorTick.bStartWithTickEnabled) SetActorTickEnabled(true);
		break;
	case EGameplayCueEvent::Removed:
		if (PrimaryActorTick.bStartWithTickEnabled) SetActorTickEnabled(false);
		break;
	default: break;
	}
	Super::HandleGameplayCue(MyTarget, EventType, Parameters);
}
