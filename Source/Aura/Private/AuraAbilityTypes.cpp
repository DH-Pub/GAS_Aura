// Copyright Hung


#include "AuraAbilityTypes.h"

void FAuraAbilityActorInfo::InitFromActor(AActor* InOwnerActor, AActor* InAvatarActor,
	UAbilitySystemComponent* InAbilitySystemComponent)
{
	FGameplayAbilityActorInfo::InitFromActor(InOwnerActor, InAvatarActor, InAbilitySystemComponent);
}

void FAuraAbilityActorInfo::ClearActorInfo()
{
	FGameplayAbilityActorInfo::ClearActorInfo();
}
