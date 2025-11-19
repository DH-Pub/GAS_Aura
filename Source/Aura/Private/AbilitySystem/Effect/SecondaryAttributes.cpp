// Copyright Hung


#include "AbilitySystem/Effect/SecondaryAttributes.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Character/AuraCharacterBase.h"

/*=========================================================================================================*/
UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef = FGameplayEffectAttributeCaptureDefinition(
		UAuraAttributeSet::GetVigorAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	// AURA_DEFINE_CAPTURE_DEF(VigorDef, GetVigorAttribute(), Target, false);
	// RelevantAttributesToCapture.Add(VigorDef);
	RelevantAttributesToCapture.Add(VigorDef);
	AURA_DEFINE_CAPTURE_DEF(StrengthDef, GetStrengthAttribute(), Target, false);
	RelevantAttributesToCapture.Add(StrengthDef);
}
float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters EvaluateParameters; // Gather tags from source and target
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Vigor = 0.f; GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluateParameters, Vigor);
	Vigor = FMath::Max<float>(Vigor, 0.f);

	float Strength = 0.f; GetCapturedAttributeMagnitude(StrengthDef, Spec, EvaluateParameters, Strength);
	Strength = FMath::Max(Strength, 0.f);

	const AAuraCharacterBase* CharacterBase = Cast<AAuraCharacterBase>(
		Spec.GetEffectContext().GetInstigatorAbilitySystemComponent()->GetAvatarActor());
	const int32 CharacterLevel = CharacterBase ? CharacterBase->GetCharacterLevel() : 1;
	return (Vigor * 2.5f) + (Strength * .5f) + 10.f * CharacterLevel;
}

/*====================================================================================================================*/
UMMC_MaxMana::UMMC_MaxMana()
{
	AURA_DEFINE_CAPTURE_DEF(IntelligenceDef, GetIntelligenceAttribute(), Target, false);
	RelevantAttributesToCapture.Add(IntelligenceDef);
}
float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters EvaluateParameters; // Gather tags from source and target
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Intelligent = 0.f; GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluateParameters, Intelligent);
	Intelligent = FMath::Max<float>(Intelligent, 0.f);

	const AAuraCharacterBase* CharacterBase = Cast<AAuraCharacterBase>(
		Spec.GetEffectContext().GetInstigatorAbilitySystemComponent()->GetAvatarActor());
	const int32 CharacterLevel = CharacterBase ? CharacterBase->GetCharacterLevel() : 1;
	return 20.f + 2.5f * Intelligent + 15.f * CharacterLevel;
}



USecondaryAttributesEffect::USecondaryAttributesEffect()
{
	/*
	 * Instant: modify BaseValue (Permanent) (ex: Damage Effect Decrease Health)
	 * HasDuration:
	 *	+ UGameplayEffect::NO_PERIOD(0.f): modify CurrentValue(Temporal) (ex: 5s buff to strength)
	 *	+ > UGameplayEffect::NO_PERIOD: modify BaseValue(Permanent) (ex: damage, heal over time)
	 * Infinite: Stay until Removed (ex: buff while the weapon is equipped, fire hazard applying debuffs while inside)
	 *	+ > UGameplayEffect::NO_PERIOD: (Permanent) ex: Health, Mana Regen
	 *
	 *	Executions: only works for (Permanent)
	 */
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	FGameplayModifierInfo Info;

	FAttributeBasedFloat AttributeBasedFloat;
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetArmorAttribute(), AddBase, .25, 2, 6, GetResilienceAttribute()) // 0
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetArmorPenetrationAttribute(), AddBase, .15, 1, 3, GetStrengthAttribute()) // 1
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetBlockChanceAttribute(), AddBase, .25, 0, 4, GetArmorAttribute()) // 2
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetCriticalHitChanceAttribute(), AddBase, .25, 0, 5, GetArmorPenetrationAttribute()) // 3
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetCriticalHitDamageAttribute(), AddBase, .15, 0, .25, GetArmorPenetrationAttribute()) // 4
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetCriticalHitResistanceAttribute(), AddBase, .25, 0, 10, GetArmorAttribute()) // 5

	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetHealthRegenerationAttribute(), AddBase, .1, 0, 1, GetVigorAttribute()) // 6
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetManaRegenerationAttribute(), AddBase, .1, 0, 1, GetIntelligenceAttribute()) // 7

	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetFireResistanceAttribute(), AddBase, .5, 0, 3, GetResilienceAttribute()) // 8
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetLightningResistanceAttribute(), AddBase, .5, 0, 3, GetResilienceAttribute()) // 9
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetArcaneResistanceAttribute(), AddBase, .5, 0, 3, GetResilienceAttribute()) // 10
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetPhysicalResistanceAttribute(), AddBase, .5, 0, 3, GetResilienceAttribute()) // 11

	FCustomCalculationBasedFloat CustomCalculation;
	AURA_ADD_CUSTOM_CALCULATION_MODIFIER(GetMaxHealthAttribute(), AddBase, UMMC_MaxHealth) // 12
	AURA_ADD_CUSTOM_CALCULATION_MODIFIER(GetMaxManaAttribute(), AddBase, UMMC_MaxMana) // 13
}
