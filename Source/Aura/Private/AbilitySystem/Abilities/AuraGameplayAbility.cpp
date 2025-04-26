// Copyright Hung


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "Interaction/CombatInterface.h"

void UAuraGameplayAbility::GetAvatarCombatInterface(TScriptInterface<ICombatInterface>& CombatInterface,
	TEnumAsByte<EOutcomeCombatInterface>& Outcome)
{
	// Outcome = EOutcomeCombatInterface::Failure;
	if (AvatarCombatInterface == nullptr)
	{
		AvatarCombatInterface = GetAvatarActorFromActorInfo();
		if (AvatarCombatInterface == nullptr)
		{
			Outcome = EOutcomeCombatInterface::Failure;
			return;
		}
	}
	CombatInterface = AvatarCombatInterface;
	Outcome = EOutcomeCombatInterface::Success;
}
