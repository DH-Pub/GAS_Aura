// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AuraGameplayEffectTypes.h"
#include "AuraAbilitySystemGlobals.generated.h"

/**
 * Project Settings: Game - Gameplay Abilities Settings -> Gameplay: Ability System Global Class
 */
UCLASS()
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override {return new FAuraGameplayEffectContext();}
};
