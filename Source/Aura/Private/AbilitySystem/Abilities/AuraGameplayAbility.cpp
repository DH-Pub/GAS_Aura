// Copyright Hung


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "Interaction/CombatInterface.h"
#include "Interaction/EnemyInterface.h"

UAuraGameplayAbility::UAuraGameplayAbility()
{
	// InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	// bRetriggerInstancedAbility = true;
}

void UAuraGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	if (AvatarActor == nullptr)
	{
		AvatarActor = GetAvatarActorFromActorInfo();
		AvatarCombatInterface = AvatarActor;
		AvatarEnemyInterface = AvatarActor;
		if (AvatarCombatInterface == nullptr) EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
	}
}
