// Copyright Hung

#pragma once

#include "CoreMinimal.h"

constexpr int32 GCustom_Depth_Red{250};

// Project Settings->Engine->Collision->Trace (DefaultEngine.ini +DefaultChannelResponses=())
#define ECC_Projectile ECollisionChannel::ECC_GameTraceChannel1
#define ECC_Mouse ECollisionChannel::ECC_GameTraceChannel2

UENUM(BlueprintType)
enum class EAuraAbilityInputID : uint8
{
	None, /* 0: nothing, this won't be used */
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
};

// this will be set as negative index for FGameplayAbilitySpec::InputID
UENUM(BlueprintType)
enum class EAuraAbilityPassiveID : uint8
{
	None, /* Nothing, this won't be used */
	INDEX_NONE, /* ::InputID default is INDEX_NONE (-1), use this for Passives and Actives not assigned to any slot*/

	Passive1,
	Passive2,
};
