// Copyright Hung


#include "AuraGameplayTags.h"


namespace AuraGameplayTags
{
#pragma region Attributes
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes,"Attributes","==PARENT==")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Strength,"Attributes.Primary.Strength","Increases physical Dmg")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Intelligence,"Attributes.Primary.Intelligence","Increases magical Dmg")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Resilience,"Attributes.Primary.Resilience","Increases Armor")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Vigor,"Attributes.Primary.Vigor","Increases Health")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_Armor,"Attributes.Secondary.Armor","Reduces Dmg taken, +BlockChance")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_ArmorPenetration,"Attributes.Secondary.ArmorPenetration","Ignore % of armor, +CritChance")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_BlockChance,"Attributes.Secondary.BlockChance","Chance to cut incoming damage in half")
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

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Cooldown_Reduce, "Ability.Cooldown.Reduce", "")

	// Abilities with Cooldown ============================================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Fire_FireBolt, "Ability.Fire.FireBolt", "FireBolt Gameplay Ability")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Lightning_Electrocute, "Ability.Lightning.Electrocute", "")
	// =======================================================================================================================

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status, "Ability.Status", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status_Locked, "Ability.Status.Locked", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status_Eligible, "Ability.Status.Eligible", "Reached Lv Requirement to unlock")
#pragma endregion

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Generic_Ability, "Generic.Ability", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Generic_Ability_Cancelable, "Generic.Ability.Cancelable",
		"Genric Tag for Ability that can be canceled when another is triggered (Dash to cancel attack, ...), before EndAbility")
	/*UE_DEFINE_GAMEPLAY_TAG_COMMENT(Generic_Ability_Blockable, "Generic.Ability.Blockable",
		"Genric Tag for Ability that can't be activated when others are actives. (ex: HitReact, ...)"
		"For bRetriggerInstancedAbility=true, it can block itself from being retrigger until EndAbility/CancelAbility")*/

#pragma region CharacterState
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_State_Ability, "Character.State.Ability",
		"Block other Abilities, ... Added in ActivationOwnedTags(To Block others including self) and ActivationBlockedTags(To be Blocked)")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_State_HitReact, "Character.State.HitReact", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_State_Death, "Character.State.Death", "")

	// PlayerTags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_State_Block_Movement, "Character.State.Block.Movement", "Block Movement")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_State_Block_Input, "Character.State.Block.Input", "Block Player's Input")
#pragma endregion

#pragma region Combat
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "Damage", "Incoming Damage from GE_Damage: ExecCalc_Damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Fire, "Damage.Fire", "Fire type damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Lightning, "Damage.Lightning", "Lightning type damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Arcane, "Damage.Arcane", "Arcane type damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Physical, "Damage.Physical", "Physical type damage")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff, "Debuff","")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Type, "Debuff.Type", "PARENT")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Type_Burn, "Debuff.Type.Burn", "Fire type Debuff")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Type_Stun, "Debuff.Type.Stun", "Lightning type debuff")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Type_Arcane, "Debuff.Type.Arcane", "Arcane type debuff")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Type_Physical, "Debuff.Type.Physical", "Physical type debuff")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEventTagsCategory_Montage_1, "GameplayEventTagsCategory.Montage.1", "Generic for SendGameplayEventToActor in montage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEventTagsCategory_Montage_2, "GameplayEventTagsCategory.Montage.2", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEventTagsCategory_Montage_3, "GameplayEventTagsCategory.Montage.3", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEventTagsCategory_Montage_4, "GameplayEventTagsCategory.Montage.4", "")
#pragma endregion


#pragma region Gameplay Cue
	// UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_FireBolt_Impact, "GameplayCue.FireBolt.Impact", "Fire Bolt explode on impact")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Damage, "GameplayCue.Damage", "Gameplay cue executed for GE_Damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Impact_Melee, "GameplayCue.Impact.Melee", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Impact_Projectile, "GameplayCue.Impact.Projectile", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_ShockBurst, "GameplayCue.ShockBurst", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Summon, "GameplayCue.Summon", "")
#pragma endregion
}


namespace MessageTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message,"Message", "Parent Tag")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_HealthCrystal,"Message.HealthCrystal", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_HealthPotion,"Message.HealthPotion", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_ManaCrystal,"Message.ManaCrystal", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_ManaPotion,"Message.ManaPotion", "")
}
