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

UDisableMovementEffect::UDisableMovementEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	UAssetTagsGameplayEffectComponent* AssetTags = CreateDefaultSubobject<UAssetTagsGameplayEffectComponent>("AssetTags");
	AssetTags->SetAndApplyAssetTagChanges(FInheritedTagContainer(FGameplayTagContainer(AuraGameplayTags::Attributes_Vital_MovementSpeed)));
	GEComponents.Add(AssetTags);
	FGameplayModifierInfo Info;
	Info.Attribute = UAuraAttributeSet::GetMovementSpeedAttribute();
	Info.ModifierOp = EGameplayModOp::Override;
	FSetByCallerFloat SetByCallerFloat;
	SetByCallerFloat.DataTag = AuraGameplayTags::Attributes_Vital_MovementSpeed;
	Info.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerFloat);
	Modifiers.Add(Info);
}
