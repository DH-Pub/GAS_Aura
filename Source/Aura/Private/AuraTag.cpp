// Copyright Hung


#include "AuraTag.h"


namespace AuraTag
{
#pragma region Attributes
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes,"Attributes","==PARENT==")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Strength,"Attributes.Primary.Strength","Increases physical Dmg")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Intelligence,"Attributes.Primary.Intelligence","Increases magical Dmg")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Resilience,"Attributes.Primary.Resilience","Increases Armor")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Vigor,"Attributes.Primary.Vigor","Increases Health")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_Armor,"Attributes.Secondary.Armor","Reduces Dmg taken, +BlockChance")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_ArmorPenetration,"Attributes.Secondary.ArmorPenetration","Ignore % of armor, +CritChance")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_BlockChance,"Attributes.Secondary.BlockChance","Chance to cut .5*DMG")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitChance,"Attributes.Secondary.CriticalHitChance","")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitDamage,"Attributes.Secondary.CriticalHitDamage","")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitResistance,"Attributes.Secondary.CriticalHitResistance","Reduce CritChance of incoming atk")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_HealthRegeneration,"Attributes.Secondary.HealthRegeneration","Periodical HP Regen")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_ManaRegeneration,"Attributes.Secondary.ManaRegeneration","Periodical MP Regen")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_MaxHealth,"Attributes.Secondary.MaxHealth","")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_MaxMana,"Attributes.Secondary.MaxMana","")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_Health, "Attributes.Vital.Health", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_Mana, "Attributes.Vital.Mana", "")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_MovementSpeed, "Attributes.Vital.MovementSpeed", "Character MovementSpeed")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Fire, "Attributes.Resistance.Fire", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Lightning, "Attributes.Resistance.Lightning", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Arcane, "Attributes.Resistance.Arcane", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Physical, "Attributes.Resistance.Physical", "")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Meta_IncomingXP, "Attributes.Meta.IncomingXP", "Receive XP")
#pragma endregion


#pragma region Ability
	// Abilities
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability, "Ability", "Parent Tag") // ==PARENT==

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack, "Ability.Attack", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Summon, "Ability.Summon", "")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Cooldown_Reduce, "Ability.Cooldown.Reduce", "Reduce remaining cooldown")
	// Abilities with Cooldown =======================================================================================
	UE_DEFINE_GAMEPLAY_TAG(Ability_Fire_FireBolt, "Ability.Fire.FireBolt")
	// UE_DEFINE_GAMEPLAY_TAG(Ability_Fire_FireBolt_Cooldown, "Ability.Fire.FireBolt.Cooldown")

	UE_DEFINE_GAMEPLAY_TAG(Ability_Lightning_Electrocute, "Ability.Lightning.Electrocute")

	UE_DEFINE_GAMEPLAY_TAG(Ability_Arcane_ArcaneShards, "Ability.Arcane.ArcaneShards")
	// ================================================================================================================

	UE_DEFINE_GAMEPLAY_TAG(Ability_Passive_HaloOfProtection, "Ability.Passive.HaloOfProtection")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Passive_LifeSiphon, "Ability.Passive.LifeSiphon")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Passive_ManaSiphon, "Ability.Passive.ManaSiphon")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status, "Ability.Status", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status_Locked, "Ability.Status.Locked", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status_Eligible, "Ability.Status.Eligible", "Reached Lv Requirement to unlock")


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Cancelable_Generic, "Ability.Cancelable.Generic", "This ability can be blocked by most things")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Cancelable_Passive_Generic, "Ability.Cancelable.Passive.Generic", "")
	// UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Blockable_Generic, "Ability.Blockable.Generic", "")
	// UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Blockable_Passive_Generic, "Ability.Blockable.Passive.Generic", "")
#pragma endregion


#pragma region Effect
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_RemoveOnDeath, "Effect.RemoveOnDeath", "Add this to Effect's AssetTags")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Ability_Cooldown, "Effect.Ability.Cooldown", "Effect Apply Cooldown")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Duration, "Effect.Duration", "Effect SetByCaller")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Period, "Effect.Period", "Effect SetByCaller")
#pragma endregion


#pragma region CharacterState
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_HitReact, "State.HitReact", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_HitReact_Stun, "State.HitReact.Stun", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_HitReact_Shocked, "State.HitReact.Shocked", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Death, "State.Death", "")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Ability, "State.Ability", "Cast Ability")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Ability_Generic, "State.Ability.Generic", "")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_HitReact_Knockback, "State.HitReact.Knockback", "For Send Event")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_HitReact_PlayMontage, "State.HitReact.PlayMontage", "For Send Event")

	UE_DEFINE_GAMEPLAY_TAG(State_Debuff_Burn, "State.Debuff.Burn")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Block_Movement, "State.Block.Movement", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Block_Movement_Speed, "State.Block.Movement.Speed", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Block_Movement_Rotation, "State.Block.Movement.Rotation", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Block_Input, "State.Block.Input", "")
#pragma endregion


#pragma region Combat
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "Damage", "Incoming Damage from GE_Damage: ExecCalc_Damage")

	UE_DEFINE_GAMEPLAY_TAG(Damage_Blocked, "Damage.Blocked")
	UE_DEFINE_GAMEPLAY_TAG(Damage_Crit, "Damage.Crit")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Fire, "Damage.Fire", "Fire type damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Lightning, "Damage.Lightning", "Lightning type damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Arcane, "Damage.Arcane", "Arcane type damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Physical, "Damage.Physical", "Physical type damage")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Knockback, "Damage.Knockback", "Damage will cause Knockback")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff, "Debuff","")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Type, "Debuff.Type", "PARENT")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Type_Burn, "Debuff.Type.Burn", "Fire type Debuff")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Type_Electric, "Debuff.Type.Electric", "Fire type Debuff")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Type_Arcane, "Debuff.Type.Arcane", "Arcane type debuff")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Type_Physical, "Debuff.Type.Physical", "Physical type debuff")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Montage, "GameplayEvent.Montage", "Generic for SendGameplayEventToActor")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Montage_1, "GameplayEvent.Montage.1", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Montage_2, "GameplayEvent.Montage.2", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Montage_3, "GameplayEvent.Montage.3", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Montage_4, "GameplayEvent.Montage.4", "")
#pragma endregion


#pragma region Cues
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Shared_Damage, "GameplayCue.Shared.Damage", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Shared_Impact_Melee, "GameplayCue.Shared.Impact.Melee", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Shared_Impact_Projectile, "GameplayCue.Shared.Impact.Projectile", "")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Shared_ShockLoop, "GameplayCue.Shared.ShockLoop", "")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Shared_Burn, "GameplayCue.Shared.Burn", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Shared_Stun, "GameplayCue.Shared.Stun", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Shared_Shocked, "GameplayCue.Shared.Shocked", "")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Shared_Summon, "GameplayCue.Shared.Summon", "")

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Melee_Pierce_Impact, "GameplayCue.Melee.Pierce.Impact")

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Shared_LevelUp, "GameplayCue.Shared.LevelUp")

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Passive_HaloOfProtection, "GameplayCue.Passive.HaloOfProtection")
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Passive_LifeSiphon, "GameplayCue.Passive.LifeSiphon")
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Passive_ManaSiphon, "GameplayCue.Passive.ManaSiphon")

	/*UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Projectile_FireBolt_Impact, "GameplayCue.Projectile.FireBolt.Impact", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Beam_Electrocute_Line, "GameplayCue.Beam.Electrocute.Line", "")*/
#pragma endregion
}


namespace MessageTags
{
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Pickup,"GameplayCue.Pickup") // ParentTag
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Pickup_HealthCrystal,"GameplayCue.Pickup.HealthCrystal")
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Pickup_HealthPotion,"GameplayCue.Pickup.HealthPotion")
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Pickup_ManaCrystal,"GameplayCue.Pickup.ManaCrystal")
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Pickup_ManaPotion,"GameplayCue.Pickup.ManaPotion")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message,"Message", "Parent Tag") //TODO: DEPRECATED, Remove this
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_HealthCrystal,"Message.HealthCrystal", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_HealthPotion,"Message.HealthPotion", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_ManaCrystal,"Message.ManaCrystal", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_ManaPotion,"Message.ManaPotion", "")
}
