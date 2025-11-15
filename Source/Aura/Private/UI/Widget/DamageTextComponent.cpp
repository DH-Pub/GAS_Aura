// Copyright Hung


#include "UI/Widget/DamageTextComponent.h"

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
