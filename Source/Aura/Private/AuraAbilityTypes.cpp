// Copyright Hung


#include "AuraAbilityTypes.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Character/AuraCharacterBase.h"

void FAuraAbilityActorInfo::InitFromActor(AActor* InOwnerActor, AActor* InAvatarActor,
	UAbilitySystemComponent* InAbilitySystemComponent)
{
	FGameplayAbilityActorInfo::InitFromActor(InOwnerActor, InAvatarActor, InAbilitySystemComponent);

	AuraAvatar = Cast<AAuraCharacterBase>(InAvatarActor);
	AuraASC = Cast<UAuraAbilitySystemComponent>(InAbilitySystemComponent);
}

void FAuraAbilityActorInfo::ClearActorInfo()
{
	FGameplayAbilityActorInfo::ClearActorInfo();

	AuraAvatar = nullptr; AuraASC = nullptr;
}
