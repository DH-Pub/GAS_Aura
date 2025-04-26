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
#pragma endregion 

	
#pragma region Input
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_LMB)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_RMB)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_1)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_2)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_3)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_4)
#pragma endregion


#pragma region Combat
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Incoming)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effects_HitReact)
#pragma endregion
	
	
#pragma region GameplayCue

#pragma endregion
}
