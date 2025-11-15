// Copyright Hung


#include "AbilitySystem/Ability/BeamAbility.h"

void UBeamAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// if (ActorInfo->PlayerController.Get()) ActorInfo->PlayerController->SetShowMouseCursor(false);
}

void UBeamAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                              const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	// if (ActorInfo->PlayerController.Get()) ActorInfo->PlayerController->SetShowMouseCursor(true);
}
