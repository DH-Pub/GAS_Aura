// Copyright Hung


#include "AbilitySystem/Effect/VitalsResetEffect.h"

#include "AbilitySystem/AuraAttributeSet.h"

UVitalsResetEffect::UVitalsResetEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo Info;
	FAttributeBasedFloat AttributeBasedFloat;
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetHealthAttribute(), Override, 1, 0, 0, GetMaxHealthAttribute())
	AURA_ADD_ATTRIBUTE_BASED_MODIFIER(GetManaAttribute(), Override, 1, 0, 0, GetMaxManaAttribute())
}
