// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "AuraGameplayCueNotify_Actor.generated.h"

UCLASS()
class AURA_API AAuraGameplayCueNotify_Actor : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
public:
	AAuraGameplayCueNotify_Actor();

	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;
};
