// Copyright Hung


#include "AuraGameplayTags.h"


namespace AuraGameplayTags
{
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

	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_LMB, "Input.MnK.LMB", "Left")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_RMB, "Input.MnK.RMB", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_1, "Input.MnK.1", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_2, "Input.MnK.2", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_3, "Input.MnK.3", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_4, "Input.MnK.4", "")
}
