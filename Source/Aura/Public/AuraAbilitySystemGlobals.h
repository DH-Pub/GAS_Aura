// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilitySystemGlobals.generated.h"

/**
 * Project Settings: Game - Gameplay Abilities Settings -> Gameplay: Ability System Global Class
 */
UCLASS()
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	static class UAuraAbilitySystemComponent* GetAuraASC(const AActor* Actor);
	// virtual FGameplayAbilityActorInfo* AllocAbilityActorInfo() const override;

	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override; //REQUIRED: for FAuraEffectContext
};
