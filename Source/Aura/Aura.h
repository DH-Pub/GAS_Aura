// Copyright Hung

#pragma once

#include "CoreMinimal.h"

constexpr int32 GCustom_Depth_Red{250};

static const FName AuraActorTag_Enemy("Enemy"); // #define AuraActorTag_Enemy FName("Enemy")
static const FName AuraActorTag_Player("Player"); // #define AuraActorTag_Player FName("Player")


// Project Settings->Engine->Collision->Trace (DefaultEngine.ini +DefaultChannelResponses=())
/** Weapons/Projectile (Radius > 0)/Area Actor apply effect to Character Capsule (bGenerateOverlapEvents = true) */
#define ECC_AuraObject_Effect ECollisionChannel::ECC_GameTraceChannel1

#define ECC_AuraTrace_Mouse ECollisionChannel::ECC_GameTraceChannel2

/** Trace used by Weapons / Bullet (LineTrace) for physics assets, not capsule */
#define ECC_AuraTrace_Effect ECollisionChannel::ECC_GameTraceChannel3
/** Trace used by Weapons / Bullet (LineTrace) overlap multiple pawns rather than blocked on first hit */
#define ECC_AuraTrace_EffectMulti ECollisionChannel::ECC_GameTraceChannel4
/** Trace used by Weapons / Bullet (LineTrace) for pawn capsule not physics assets */
#define ECC_AuraTrace_EffectCapsule ECollisionChannel::ECC_GameTraceChannel5


enum EAuraSurface : int
{
	Default = EPhysicalSurface::SurfaceType_Default,
	Character = SurfaceType1,
	CharacterGreen = SurfaceType2,
};

/**
 * For AbilitySpec's InputID 0 to 255
 */
UENUM(BlueprintType)
namespace EAuraAbilityInputID
{
	enum Type : uint8
	{
		None = 0, // Nothing, this won't be used

		Confirm, /* 1 */
		Cancel, /* 2 */

		Ability1,
		Ability2,
		Ability3,
		Ability4,
		Ability5,
		Ability6,

		Ctrl, // Testing
		Shift, // Testing

		/**
		 * Passives for Ability Slots
		 */
		PassiveForAbilitySlots = 100,
		Passive1,
		Passive2,
	};
}

/*UENUM(BlueprintType)
enum class EAuraAbilityInputID : uint8
{
	None, /* 0: nothing, this won't be used #1#
	Confirm, /* 1 #1#
	Cancel, /* 2 #1#

	Ability1,
	Ability2,
	Ability3,
	Ability4,
	Ability5,
	Ability6,

	Ctrl, // Testing
	Shift, // Testing
};*/
// this will be set as negative index for FGameplayAbilitySpec::InputID -0 to -255
/*UENUM(BlueprintType)
enum class EAuraAbilityPassiveID : uint8
{
	None, /* Nothing, this won't be used #1#
	INDEX_NONE,

	Passive1,
	Passive2,
};*/


UENUM(BlueprintType)
enum class EAuraHitDirection : uint8
{
	None,
	Front,
	Back,
	Left,
	Right,
};
