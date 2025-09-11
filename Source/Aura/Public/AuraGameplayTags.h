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

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Meta_IncomingXP)
#pragma endregion 


#pragma region Input
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input) // ==PARENT==
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_UI_AttributeMenu)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Move_Directional)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Move_Mouse)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Combat_Ability_1)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Combat_Ability_2)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Combat_Ability_3)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Combat_Ability_4)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Combat_Ability_5)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Combat_Ability_6)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Combat_Passive_1)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Combat_Passive_2)
#pragma endregion 


#pragma region Ability
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability)

	// Asset Tags
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Move)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Move_Directional)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Move_Mouse)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Move_Auto)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_HitReact)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Summon)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Cooldown_Reduce)
	// Abilities with CD =============================================================================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Fire_FireBolt)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Fire_FireBolt_Cooldown) // CD Tags

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Lightning_Electrocute)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Lightning_Electrocute_Cooldown)
	//================================================================================================================

	// Dynamic Tags
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Status)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Status_Locked)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Status_Eligible) // Reached Level requirement to unlock this Ability
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Status_Unlocked)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Activatable)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive)
#pragma endregion

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Generic_Ability)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Generic_Ability_Cancelable)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Generic_Ability_Blockable)



#pragma region Combat
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Fire)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Lightning)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Arcane)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Physical)
	/*static const TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances = {
		{Damage_Fire, Attributes_Resistance_Fire},
		{Damage_Lightning, Attributes_Resistance_Lightning},
		{Damage_Arcane, Attributes_Resistance_Arcane},
		{Damage_Physical, Attributes_Resistance_Physical},
	};*/
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Burn)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Stun)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Arcane)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Physical)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Montage_Attack_1)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Montage_Attack_2)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Montage_Attack_3)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Montage_Attack_4)
#pragma endregion


#pragma region GameplayCue
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_FireBolt_Impact)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Impact_Melee)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Impact_Projectile)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Summon)
#pragma endregion
}


namespace MessageTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_HealthCrystal)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_HealthPotion)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_ManaCrystal)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_ManaPotion)
}
