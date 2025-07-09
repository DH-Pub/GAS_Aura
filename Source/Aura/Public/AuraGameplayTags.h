// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"


namespace AuraGameplayTags
{
	// Automatically add to Gameplay Tag List
#pragma region Attributes
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Strength)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Intelligence)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Resilience)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Vigor)
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_Armor)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_ArmorPenetration)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_BlockChance)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CriticalHitChance)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CriticalHitDamage)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CriticalHitResistance)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_HealthRegeneration)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_ManaRegeneration)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_MaxHealth)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_MaxMana)
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Vital_Health)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Vital_Mana)
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Fire)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Lightning)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Arcane)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Physical)
#pragma endregion 

	
#pragma region Input
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_LMB)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_RMB)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_1)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_2)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_3)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_4)
#pragma endregion


#pragma region Combat
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Fire)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Lightning)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Arcane)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Physical)
	static const TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances = {
		{Damage_Fire, Attributes_Resistance_Fire},
		{Damage_Lightning, Attributes_Resistance_Lightning},
		{Damage_Arcane, Attributes_Resistance_Arcane},
		{Damage_Physical, Attributes_Resistance_Physical},
	};

	//TODO next proj: Change to singular
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Attack)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Summon)

	
	// Abilities with CD =============================================================================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Fire_FireBolt)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Fire_FireBolt)
	//================================================================================================================
	
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(CombatSocket_Weapon)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(CombatSocket_RightHand)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(CombatSocket_LeftHand)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(CombatSocket_Tail)
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Montage_Attack_1)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Montage_Attack_2)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Montage_Attack_3)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Montage_Attack_4)
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effects_HitReact)
#pragma endregion
	
	
#pragma region GameplayCue
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_FireBolt_Impact)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Impact_Melee)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Impact_Projectile)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Summon)
#pragma endregion
}
