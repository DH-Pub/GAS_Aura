// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"


// Project Settings -> Project - GameplayTags -> ✅Fast Replication & Add "Commonly Replicated Tags"

namespace AuraTag // Automatically add to Gameplay Tag List
{
#pragma region Attributes
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes)
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

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Vital_MovementSpeed)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Fire)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Lightning)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Arcane)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Physical)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Meta_IncomingXP)
#pragma endregion


#pragma region Ability
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Summon)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Cooldown_Reduce) // Activate this to reduce remaining cooldown of abilities
	// Abilities with CD =============================================================================================
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Fire_FireBolt) // UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Fire_FireBolt_Cooldown)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Lightning_Electrocute)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Arcane_ArcaneShards)
	//================================================================================================================

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Passive_HaloOfProtection)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Passive_LifeSiphon)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Passive_ManaSiphon)

	// Dynamic Tags
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Status) //TODO: Delete all Status this
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Status_Locked)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Status_Eligible) // Reached Level requirement to unlock this Ability


	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Cancelable_Generic) // This Active Ability can be canceled by most
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Cancelable_Passive_Generic)
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Blockable_Generic)
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Blockable_Passive_Generic)
#pragma endregion


#pragma region Effect
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_RemoveOnDeath)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Ability_Cooldown)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Duration)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Period)
#pragma endregion


#pragma region Character State // ActivationOwnedTags(Block others including self) and ActivationBlockedTags(Be Blocked)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_HitReact)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_HitReact_Stun)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_HitReact_Shocked)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Death)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Ability) // Cast Ability State
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Ability_Generic)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_HitReact_Knockback) // For Send Event
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_HitReact_PlayMontage) // For Send Event

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Debuff_Burn)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Block_Movement)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Block_Movement_Speed)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Block_Movement_Rotation)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Block_Input)
#pragma endregion


#pragma region Combat
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Blocked)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Crit)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Fire)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Lightning)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Arcane)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Physical)
	static const FGameplayTagContainer DamageTypeContainer = FGameplayTagContainer::CreateFromArray<FDefaultAllocator>({
		Damage_Fire, Damage_Lightning, Damage_Arcane, Damage_Physical
	});

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Knockback)

	/*static const TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances = {
		{Damage_Fire, Attributes_Resistance_Fire},
		{Damage_Lightning, Attributes_Resistance_Lightning},
		{Damage_Arcane, Attributes_Resistance_Arcane},
		{Damage_Physical, Attributes_Resistance_Physical},
	};*/
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Type)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Type_Burn)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Type_Electric)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Type_Arcane)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff_Type_Physical)


	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Montage) // Generic GameplayEvents for Animation Montage
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Montage_1)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Montage_2)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Montage_3)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Montage_4)
#pragma endregion


#pragma region GameplayCue
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_Damage)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_Impact_Melee)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_Impact_Projectile)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_ShockLoop)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_Burn)
	/**
	 * - Effect with this GameplayEffectCue can't be periodical: so that cue not execute after
	 * EGameplayCueEvent::Removed, which will create new GCActor
	 * - Must be stackable: to avoid repeatedly remove, add GC, which may add multiple GCActor and all GameplayCue is not removed
	 */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_Stun)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_Shocked)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_Summon)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Melee_Pierce_Impact)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Spell_ArcaneShards)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_LevelUp)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Passive_HaloOfProtection)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Passive_LifeSiphon)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Passive_ManaSiphon)
#pragma endregion
}


namespace MessageTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Pickup)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Pickup_HealthCrystal)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Pickup_HealthPotion)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Pickup_ManaCrystal)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Pickup_ManaPotion)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message) //TODO: DEPRECATED, Remove this
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_HealthCrystal)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_HealthPotion)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_ManaCrystal)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_ManaPotion)
}
