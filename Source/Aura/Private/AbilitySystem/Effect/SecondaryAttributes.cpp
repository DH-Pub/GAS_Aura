// Copyright Hung


#include "AbilitySystem/Effect/SecondaryAttributes.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Character/AuraCharacterBase.h"

/*=========================================================================================================*/
UMMC_MaxHealth::UMMC_MaxHealth()
{
	AURA_ATTR_DEFINE_CAPTUREDEF(VigorDef, GetVigorAttribute(), Target, false);
	RelevantAttributesToCapture.Add(VigorDef);
	AURA_ATTR_DEFINE_CAPTUREDEF(StrengthDef, GetStrengthAttribute(), Target, false);
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
	AURA_ATTR_DEFINE_CAPTUREDEF(IntelligenceDef, GetIntelligenceAttribute(), Target, false);
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
	/** BaseValue == Permanent; CurrentValue = BaseValue + Temporary Modification from GE
	 * Instant: modify BaseValue(Permanent) (ex: Damage Effect Decrease Health)
	 * HasDuration: CurrentValue(Temporal) (ex: 5s buff to strength)
	 *	if (Period > 0.f) BaseValue (ex: damage, heal over time)
	 * Infinite: CurrentValue until Removed (ex: Equipments' Attributes, Swamp slow inside)
	 *	if (Period > 0.f): BaseValue (ex: Health, Mana, Stamina Regen, fire apply dmg while inside)
	 *
	 *	Executions: only works for (Permanent)
	 */
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetArmorAttribute(), AddBase, .25f, 2.f, 6.f, GetResilienceAttribute()) // 0
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetArmorPenetrationAttribute(), AddBase, .15f, 1.f, 3.f, GetStrengthAttribute()) // 1
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetBlockChanceAttribute(), AddBase, .25f, 0.f, 4.f, GetArmorAttribute()) // 2
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetCriticalHitChanceAttribute(), AddBase, .25f, 0.f, 5.f, GetArmorPenetrationAttribute()) // 3
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetCriticalHitDamageAttribute(), AddBase, .15f, 0.f, .25f, GetArmorPenetrationAttribute()) // 4
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetCriticalHitResistanceAttribute(), AddBase, .25f, 0.f, 10.f, GetArmorAttribute()) // 5

	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetHealthRegenerationAttribute(), AddBase, .1f, 0.f, 1.f, GetVigorAttribute()) // 6
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetManaRegenerationAttribute(), AddBase, .1f, 0.f, 1.f, GetIntelligenceAttribute()) // 7

	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetFireResistanceAttribute(), AddBase, .5f, 0.f, 3.f, GetResilienceAttribute()) // 8
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetLightningResistanceAttribute(), AddBase, .5f, 0, 3.f, GetResilienceAttribute()) // 9
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetArcaneResistanceAttribute(), AddBase, .5f, 0.f, 3.f, GetResilienceAttribute()) // 10
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetPhysicalResistanceAttribute(), AddBase, .5f, 0.f, 3.f, GetResilienceAttribute()) // 11

	AURA_ADD_CUSTOM_CALCULATION_MODIFIER(GetMaxHealthAttribute(), AddBase, UMMC_MaxHealth) // 12
	AURA_ADD_CUSTOM_CALCULATION_MODIFIER(GetMaxManaAttribute(), AddBase, UMMC_MaxMana) // 13
}
