// Copyright Hung


#include "AuraGameplayTags.h"


namespace AuraGameplayTags
{
#pragma region Attributes
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Strength,"Attributes.Primary.Strength","Increases physical damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Intelligence,"Attributes.Primary.Intelligence","Increases magical damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Resilience,"Attributes.Primary.Resilience","Increases armor and armor penetration")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Vigor,"Attributes.Primary.Vigor","Increases Health")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_Armor,"Attributes.Secondary.Armor","Reduces damage taken, improves Block Chance")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_ArmorPenetration,"Attributes.Secondary.ArmorPenetration","Ignore percentage of armor, increases crit chance")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_BlockChance,"Attributes.Secondary.BlockChance","Chance to cut incoming damage in half")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitChance,"Attributes.Secondary.CriticalHitChance","Chance to double damage plus crit bonus")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitDamage,"Attributes.Secondary.CriticalHitDamage","Bonus damage added when a critical hit is scored")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitResistance,"Attributes.Secondary.CriticalHitResistance","Reduce Critical Hit Chance of attacking enemies")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_HealthRegeneration,"Attributes.Secondary.HealthRegeneration","Amount of Health regenerated every 1 second")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_ManaRegeneration,"Attributes.Secondary.ManaRegeneration","Amount of Mana regenerated every 1 second")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_MaxHealth,"Attributes.Secondary.MaxHealth","Maximum amount of health")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_MaxMana,"Attributes.Secondary.MaxMana","Maximum amount of mana")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_Health, "Attributes.Vital.Health", "Amount of damage a player can take before death")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_Mana, "Attributes.Vital.Mana", "Amount of energy used to cast spells")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Fire, "Attributes.Resistance.Fire", "Resistance to Fire")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Lightning, "Attributes.Resistance.Lightning", "Resistance to Lightning")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Arcane, "Attributes.Resistance.Arcane", "Resistance to Arcane")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Physical, "Attributes.Resistance.Physical", "Resistance to Physical")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Meta_IncomingXP, "Attributes.Meta.IncomingXP", "Receive XP")
#pragma endregion


#pragma region Input
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input, "Input", "") // ==PARENT==
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_UI_AttributeMenu, "Input.UI.AttributeMenu", "")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Move_Directional, "Input.Move.Directional", "Keyboard, Gamepad")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Move_Mouse, "Input.Move.Mouse", "")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Combat_Ability_1, "Input.Combat.Ability.1", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Combat_Ability_2, "Input.Combat.Ability.2", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Combat_Ability_3, "Input.Combat.Ability.3", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Combat_Ability_4, "Input.Combat.Ability.4", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Combat_Ability_5, "Input.Combat.Ability.5", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Combat_Ability_6, "Input.Combat.Ability.6", "")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Combat_Passive_1, "Input.Combat.Passive.1", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Combat_Passive_2, "Input.Combat.Passive.2", "")
#pragma endregion


#pragma region Ability
	// Abilities
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability, "Ability", "Parent Tag") // ==PARENT==
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Move, "Ability.Move", "CharacterMovement Ability parent")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Move_Directional, "Ability.Move.Directional", "CharacterMovement Ability using keyboard, gamepad")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Move_Mouse, "Ability.Move.Mouse", "CharacterMovement Ability using mouse")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Move_Auto, "Ability.Move.Auto", "For AI, on player, Mouse will activate this")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_HitReact, "Ability.HitReact", "")
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack, "Ability.Attack", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Summon, "Ability.Summon", "")
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Cooldown_Reduce, "Ability.Cooldown.Reduce", "")
	
	// Abilities with Cooldown ============================================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Fire_FireBolt, "Ability.Fire.FireBolt", "FireBolt Gameplay Ability")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Fire_FireBolt_Cooldown, "Ability.Fire.FireBolt.Cooldown", "FireBolt Cooldown")
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Lightning_Electrocute, "Ability.Lightning.Electrocute", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Lightning_Electrocute_Cooldown, "Ability.Lightning.Electrocute.Cooldown", "")
	// =======================================================================================================================
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status, "Ability.Status", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status_Locked, "Ability.Status.Locked", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status_Eligible, "Ability.Status.Eligible", "Reached Level Requirement to unlock this Ability")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status_Unlocked, "Ability.Status.Unlocked", "")
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Type_Activatable, "Ability.Type.Activatable", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Type_Passive, "Ability.Type.Passive", "")
#pragma endregion

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Generic_Ability, "Generic.Ability", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Generic_Ability_Cancelable, "Generic.Ability.Cancelable",
		"Genric Tag for Ability that can be canceled"
		"Ability can be canceled when another is triggered (Dash to cancel attack, ...), before EndAbility")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Generic_Ability_Blockable, "Generic.Ability.Blockable",
		"Genric Tag for Ability that can't be activated when others are actives. (ex: HitReact, ...)"
		"For bRetriggeredInstancedAbility=true, it can block itself from being retrigger until EndAbility/CancelAbility")


#pragma region Combat
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "Damage", "Incoming Damage pass from UAuraGameplayAbility (UAuraProjectileAbility,...) to GE_Damage: ExecCalc_Damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Fire, "Damage.Fire", "Fire type damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Lightning, "Damage.Lightning", "Lightning type damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Arcane, "Damage.Arcane", "Arcane type damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Physical, "Damage.Physical", "Physical type damage")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff, "Debuff","")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Burn, "Debuff_Burn", "Fire type Debuff")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Stun, "Debuff_Stun", "Lightning type debuff")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Arcane, "Debuff_Arcane", "Arcane type debuff")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Physical, "Debuff_Physical", "Physical type debuff")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Montage_Attack_1, "Montage.Attack.1", "Attack 1")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Montage_Attack_2, "Montage.Attack.2", "Attack 2")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Montage_Attack_3, "Montage.Attack.3", "Attack 3")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Montage_Attack_4, "Montage.Attack.4", "Attack 4")
#pragma endregion


#pragma region Gameplay Cue
	// UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_FireBolt_Impact, "GameplayCue.FireBolt.Impact", "Fire Bolt explode on impact")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Impact_Melee, "GameplayCue.Impact.Melee", "Impact Melee")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Impact_Projectile, "GameplayCue.Impact.Projectile", "Impact Projectile")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Summon, "GameplayCue.Summon", "Summon Cue")
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
