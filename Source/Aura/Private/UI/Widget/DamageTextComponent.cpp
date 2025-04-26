// Copyright Hung


#include "UI/Widget/DamageTextComponent.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Character/AuraCharacterBase.h"
#include "Net/UnrealNetwork.h"

UDamageTextComponent::UDamageTextComponent()
{
	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawAtDesiredSize(true);
	SetIsReplicatedByDefault(true);
}

void UDamageTextComponent::SetDamageText_Implementation(float Damage)
{
	BP_SetDamageText(Damage);
}
