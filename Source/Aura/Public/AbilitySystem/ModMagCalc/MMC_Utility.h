// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Utility.generated.h"

/**
 * MaxHealth
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
