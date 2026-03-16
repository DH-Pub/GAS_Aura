// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_CooldownDuration.generated.h"

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
