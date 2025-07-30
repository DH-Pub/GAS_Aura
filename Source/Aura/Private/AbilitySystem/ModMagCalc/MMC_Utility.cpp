// Copyright Hung


#include "AbilitySystem/ModMagCalc/MMC_Utility.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"


UMMC_MaxHealth::UMMC_MaxHealth()
{
	// VigorDef = FGameplayEffectAttributeCaptureDefinition(UAuraAttributeSet::GetVigorAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(VigorDef);
}
float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluateParameters, Vigor);
	Vigor = FMath::Max<float>(Vigor, 0.f);

	int32 CharacterLevel = 1;
	if(Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		CharacterLevel = ICombatInterface::Execute_GetCharacterLevel(Spec.GetContext().GetSourceObject());
	}
	
	return 2.5f * Vigor + 10.f * CharacterLevel;
}


/*====================================================================================================================*/
UMMC_MaxMana::UMMC_MaxMana()
{
	IntelligenceDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute();
	IntelligenceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntelligenceDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(IntelligenceDef);
}
float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Intelligent = 0.f;
	GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluateParameters, Intelligent);
	Intelligent = FMath::Max<float>(Intelligent, 0.f);

	int32 CharacterLevel = 1;
	if(Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		CharacterLevel = ICombatInterface::Execute_GetCharacterLevel(Spec.GetContext().GetSourceObject());
	}
	
	return 20.f + 2.5f * Intelligent + 15.f * CharacterLevel;
}


/*====================================================================================================================*/
UMMC_CooldownDuration::UMMC_CooldownDuration()
{
	IntelligenceDef = FGameplayEffectAttributeCaptureDefinition(
		UAuraAttributeSet::GetIntelligenceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	RelevantAttributesToCapture.Add(IntelligenceDef);
}
float UMMC_CooldownDuration::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const UAuraGameplayAbility* Ability = Cast<UAuraGameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated());
	if (!Ability) return 0.f;
	
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float INT = 0.f;
	GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluateParameters, INT);
	const float CooldownReductionPercentage = FMath::Min<float>(INT * 0.01f, 0.6f);
	const float CooldownDuration = Ability->CooldownDuration.GetValueAtLevel(Ability->GetAbilityLevel());
	
	return CooldownDuration * (1 - CooldownReductionPercentage);
}


/*====================================================================================================================*/
UMMC_AbilityCost::UMMC_AbilityCost()
{
}
float UMMC_AbilityCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	if (const UAuraGameplayAbility* Ability = Cast<UAuraGameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated()))
	{
		return Ability->Cost.GetValueAtLevel(Ability->GetAbilityLevel());
	}
	return 0.f;
}
