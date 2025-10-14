// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayModMagnitudeCalculation.h"
#include "CostCooldownEffect.generated.h"

/**
 * Cost Mana =======================================================================================================
 */
UCLASS()
class UMMC_AbilityManaCost : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UMMC_AbilityManaCost();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

/**
 * Cost Health =======================================================================================================
 */
UCLASS()
class UMMC_AbilityHealthCost : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UMMC_AbilityHealthCost();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
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
	/* For UI Display */
	TPair<float, float> GetBaseCooldownAndReductionPercent(const FGameplayEffectSpec& Spec) const;
private:
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
	float GetCooldownReductionPercent(const float Intelligence) const;
};


/**
 * 
 */
UCLASS()
class AURA_API UCostEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UCostEffect();
};

/**
 * 
 */
UCLASS()
class AURA_API UCooldownEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UCooldownEffect();
};
