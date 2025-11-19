// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "MovementSpeedEffect.generated.h"

/**
 * Apply during BeginPlay
 */
UCLASS()
class AURA_API UStartingMovementSpeedEffect : public UGameplayEffect
{
	GENERATED_BODY()
	UStartingMovementSpeedEffect();
};
