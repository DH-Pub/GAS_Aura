// Copyright Hung


#include "AbilitySystem/Abilities/MeleeDamageAbility.h"

#include "AuraGameplayTags.h"

UMeleeDamageAbility::UMeleeDamageAbility()
{
	bStagger = true;
	bMaxSpeedZeroedOnActivated = true;
	DamageTypes.Add(AuraGameplayTags::Damage_Physical);
}
