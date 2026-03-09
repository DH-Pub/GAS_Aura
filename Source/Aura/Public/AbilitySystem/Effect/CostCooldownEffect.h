// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayModMagnitudeCalculation.h"
#include "CostCooldownEffect.generated.h"

/**
 * Calculate and apply cost
 */
UCLASS()
class UExecCalc_AbilityCosts : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UExecCalc_AbilityCosts();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};


/**
 * Cooldown =======================================================================================================
 */
UCLASS()
class AURA_API UMMC_CooldownDuration : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UMMC_CooldownDuration();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	static float GetCooldownReductionPercent(const float Intelligence);
private:
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
};


/**
 * Cost Cooldown GameplayEffect ==============================================================================
 */
UCLASS()
class AURA_API UCostEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UCostEffect();
};
UCLASS()
class AURA_API UCooldownEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UCooldownEffect();
};
