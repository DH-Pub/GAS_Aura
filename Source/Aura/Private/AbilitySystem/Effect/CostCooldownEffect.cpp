// Copyright Hung


#include "AbilitySystem/Effect/CostCooldownEffect.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Ability/CostCooldownAbility.h"

UExecCalc_AbilityCosts::UExecCalc_AbilityCosts()
{
}
void UExecCalc_AbilityCosts::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	if (const UCostCooldownAbility* Ability = Cast<UCostCooldownAbility>(
		Spec.GetEffectContext().GetAbilityInstance_NotReplicated()))
	{
		FGameplayModifierEvaluatedData Data; Data.ModifierOp = EGameplayModOp::AddBase;
		for (auto& [Tag, ScalableCost] : Ability->AbilityCosts)
		{
			Data.Magnitude = -ScalableCost.GetValueAtLevel(Spec.GetLevel());
			if (Tag.MatchesTagExact(AuraGameplayTags::Attributes_Vital_Mana))
			{
				Data.Attribute = UAuraAttributeSet::GetManaAttribute();
			}
			else if (Tag.MatchesTagExact(AuraGameplayTags::Attributes_Vital_Health))
			{
				Data.Attribute = UAuraAttributeSet::GetHealthAttribute();
			}
			OutExecutionOutput.AddOutputModifier(Data);
		}
	}
}
/*float UMMC_AbilityManaCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	if (const UCostCooldownAbility* Ability = Cast<UCostCooldownAbility>(Spec.GetEffectContext().GetAbilityInstance_NotReplicated()))
	{return Ability->ManaCost.GetValueAtLevel(Spec.GetLevel());}
	return 0.f;
}*/


/*========================================= Cooldown MMC=============================================================*/
UMMC_CooldownDuration::UMMC_CooldownDuration()
{
	AURA_ATTR_DEFINE_CAPTUREDEF(IntelligenceDef, GetIntelligenceAttribute(), Target, false);
	RelevantAttributesToCapture.Add(IntelligenceDef);
}
float UMMC_CooldownDuration::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float* BaseCD = Spec.SetByCallerTagMagnitudes.Find(TAG_Cooldown_Duration);
	if (!BaseCD) return 0.f;
	const float CooldownDuration = *BaseCD;

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


/*
 * ==================================== Cost Cooldown ============================================
 */
UCostEffect::UCostEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	/*FGameplayModifierInfo Info; FCustomCalculationBasedFloat CustomCalculation; CustomCalculation.Coefficient = -1.f;
	AURA_ADD_CUSTOM_CALCULATION_MODIFIER(GetManaAttribute(), AddBase, UMMC_AbilityManaCost)*/

	/*const int32 i = Executions.Add(FGameplayEffectExecutionDefinition());
	Executions[i].CalculationClass = UExecCalc_AbilityCosts::StaticClass();*/
	FGameplayEffectExecutionDefinition& Def = Executions.AddDefaulted_GetRef();
	Def.CalculationClass = UExecCalc_AbilityCosts::StaticClass();
}
UCooldownEffect::UCooldownEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat CustomCalculation;
	CustomCalculation.CalculationClassMagnitude = UMMC_CooldownDuration::StaticClass();
	DurationMagnitude = FGameplayEffectModifierMagnitude(CustomCalculation);
}
