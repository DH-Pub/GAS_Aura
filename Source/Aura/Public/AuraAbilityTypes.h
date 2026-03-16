// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "UObject/Object.h"
#include "AuraAbilityTypes.generated.h"


/*
 * override AllocAbilityActorInfo in UAuraAbilitySystemGlobals
 * TODO: This is not being used
 */
USTRUCT(BlueprintType)
struct FAuraAbilityActorInfo : public FGameplayAbilityActorInfo
{
	GENERATED_BODY()

	FAuraAbilityActorInfo(){}

	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr<class AAuraCharacterBase> AuraAvatar;

	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr<class UAuraAbilitySystemComponent> AuraASC;

	virtual void InitFromActor(AActor* InOwnerActor, AActor* InAvatarActor,
		UAbilitySystemComponent* InAbilitySystemComponent) override;
	// virtual void SetAvatarActor(AActor* AvatarActor) override;
	virtual void ClearActorInfo() override;
};
