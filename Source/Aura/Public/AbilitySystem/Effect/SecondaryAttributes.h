// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayModMagnitudeCalculation.h"
#include "SecondaryAttributes.generated.h"

/**
 * MaxHealth =======================================================================================================
 */
UCLASS()
class AURA_API UMMC_MaxHealth : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UMMC_MaxHealth();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
private:
	FGameplayEffectAttributeCaptureDefinition VigorDef;
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
};

/**
 * MaxMana =======================================================================================================
 */
UCLASS()
class AURA_API UMMC_MaxMana : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UMMC_MaxMana();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
private:
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
};


/**
 *
 */
UCLASS()
class AURA_API USecondaryAttributesEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	USecondaryAttributesEffect();
};
