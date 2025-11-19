// Copyright Hung


#include "AbilitySystem/Effect/MovementSpeedEffect.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "GameplayEffectComponents/AssetTagsGameplayEffectComponent.h"

UStartingMovementSpeedEffect::UStartingMovementSpeedEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo Info;
	Info.Attribute = UAuraAttributeSet::GetMovementSpeedAttribute();
	Info.ModifierOp = EGameplayModOp::Override;
	FSetByCallerFloat SetByCallerFloat;
	SetByCallerFloat.DataTag = AuraGameplayTags::Attributes_Vital_MovementSpeed;
	Info.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerFloat);
	Modifiers.Add(Info);
}
