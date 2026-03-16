// Copyright Hung


#include "AbilitySystem/ModMagCalc/MMC_CooldownDuration.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Ability/CostCooldownAbility.h"

UMMC_CooldownDuration::UMMC_CooldownDuration()
{
	AURA_ATTR_DEFINE_CAPTUREDEF(IntelligenceDef, GetIntelligenceAttribute(), Target, false);
	RelevantAttributesToCapture.Add(IntelligenceDef);
}
float UMMC_CooldownDuration::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float* BaseCD = Spec.SetByCallerTagMagnitudes.Find(TAG_Cooldown_Duration);
	// if (!BaseCD) return 0.f;
	const float CooldownDuration = BaseCD ? *BaseCD : 0.f;

	FAggregatorEvaluateParameters EvaluateParameters; // Gather tags from source and target
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Intelligence = 0.f; GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluateParameters, Intelligence);
	return FMath::Max(CooldownDuration * (1 - GetCooldownReductionPercent(Intelligence)), .006f);
}
float UMMC_CooldownDuration::GetCooldownReductionPercent(const float Intelligence)
{
	return FMath::Min<float>(Intelligence * .01f, .6f); // Max 60%
}
