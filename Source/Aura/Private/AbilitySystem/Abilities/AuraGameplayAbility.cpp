// Copyright Hung


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "Interaction/CombatInterface.h"
#include "Interaction/EnemyInterface.h"

UAuraGameplayAbility::UAuraGameplayAbility()
{
	// InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	// bRetriggerInstancedAbility = true;
}

void UAuraGameplayAbility::GetAvatarCombatInterface(TScriptInterface<ICombatInterface>& CombatInterface,
                                                    TEnumAsByte<EOutcome>& Outcome)
{
	// Outcome = EOutcomeCombatInterface::Failure;
	if (AvatarCombatInterface == nullptr)
	{
		AvatarCombatInterface = GetAvatarActorFromActorInfo();
		if (AvatarCombatInterface == nullptr)
		{
			Outcome = EOutcome::Failure;
			return;
		}
	}
	CombatInterface = AvatarCombatInterface;
	Outcome = EOutcome::Success;
}

void UAuraGameplayAbility::GetAvatarInterfaces(TEnumAsByte<EOutcome>& Outcome,
	TScriptInterface<ICombatInterface>& CombatInterface, TScriptInterface<IEnemyInterface>& EnemyInterface)
{
	if (AvatarCombatInterface == nullptr || AvatarEnemyInterface == nullptr)
	{
		AActor* AvatarActor = GetAvatarActorFromActorInfo();
		AvatarCombatInterface = AvatarActor;
		AvatarEnemyInterface = AvatarActor;
		if (AvatarCombatInterface == nullptr || AvatarEnemyInterface == nullptr)
		{
			Outcome = EOutcome::Failure;
			return;
		}
	}
	CombatInterface = AvatarCombatInterface;
	EnemyInterface = AvatarEnemyInterface;
	Outcome = EOutcome::Success;
}
