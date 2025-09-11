// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Utility.generated.h"

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
 * Cooldown =======================================================================================================
 */
struct FCooldown
{
	
};
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
