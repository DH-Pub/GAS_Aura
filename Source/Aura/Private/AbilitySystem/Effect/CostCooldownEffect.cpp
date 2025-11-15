// Copyright Hung


#include "AbilitySystem/Effect/CostCooldownEffect.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Ability/CostCooldownAbility.h"

/*====================================================================================================================*/
UMMC_AbilityManaCost::UMMC_AbilityManaCost()
{
}
float UMMC_AbilityManaCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	if (const UCostCooldownAbility* Ability = Cast<UCostCooldownAbility>(Spec.GetEffectContext().GetAbilityInstance_NotReplicated()))
	{
		return Ability->ManaCost.GetValueAtLevel(Spec.GetLevel());
	}
	return 0.f;
}
/*====================================================================================================================*/
UMMC_AbilityHealthCost::UMMC_AbilityHealthCost()
{
}
float UMMC_AbilityHealthCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	if (const UCostCooldownAbility* Ability = Cast<UCostCooldownAbility>(Spec.GetEffectContext().GetAbilityInstance_NotReplicated()))
	{
		return Ability->HealthCost.GetValueAtLevel(Spec.GetLevel());
	}
	return 0.f;
}

/*========================================= Cooldown MMC=============================================================*/
UMMC_CooldownDuration::UMMC_CooldownDuration()
{
	AURA_DEFINE_CAPTURE_DEF(IntelligenceDef, GetIntelligenceAttribute(), Target, false);
	RelevantAttributesToCapture.Add(IntelligenceDef);
}
float UMMC_CooldownDuration::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const UCostCooldownAbility* Ability = Cast<UCostCooldownAbility>(Spec.GetEffectContext().GetAbilityInstance_NotReplicated());
	if (!Ability) return .006f;
	
	// Gather tags from source and target
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	float Intelligence = 0.f; GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluateParameters, Intelligence);
	const float CooldownDuration = Ability->CooldownDuration.GetValueAtLevel(Spec.GetLevel());
	return FMath::Max(CooldownDuration * (1 - GetCooldownReductionPercent(Intelligence)), .006f);
}
TPair<float, float> UMMC_CooldownDuration::GetBaseCooldownAndReductionPercent(const FGameplayEffectSpec& Spec) const
{
	const UCostCooldownAbility* Ability = Cast<UCostCooldownAbility>(Spec.GetEffectContext().GetAbilityInstance_NotReplicated());
	if (!Ability) return TPair<float, float>();
	const float Intelligence = Ability->GetAbilitySystemComponentFromActorInfo()->
		GetNumericAttribute(UAuraAttributeSet::GetIntelligenceAttribute());
	const float BaseCooldown = Ability->CooldownDuration.GetValueAtLevel(Spec.GetLevel());
	return TPair<float, float>(BaseCooldown, GetCooldownReductionPercent(Intelligence));
}
float UMMC_CooldownDuration::GetCooldownReductionPercent(const float Intelligence) const
{
	return FMath::Min<float>(Intelligence * .01f, .6f); // Max 60%
}


/*
 * =============================================Cost Cooldown==========================================================
 */
UCostEffect::UCostEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo Info;
	FCustomCalculationBasedFloat CustomCalculation;
	CustomCalculation.Coefficient = -1.f;
	AURA_ADD_CUSTOM_CALCULATION_MODIFIER(GetManaAttribute(), AddBase, UMMC_AbilityManaCost)
	AURA_ADD_CUSTOM_CALCULATION_MODIFIER(GetHealthAttribute(), AddBase, UMMC_AbilityHealthCost)
}
UCooldownEffect::UCooldownEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FGameplayModifierInfo Info;
	FCustomCalculationBasedFloat CustomCalculation;
	CustomCalculation.CalculationClassMagnitude = UMMC_CooldownDuration::StaticClass();
	DurationMagnitude = FGameplayEffectModifierMagnitude(CustomCalculation);
}
