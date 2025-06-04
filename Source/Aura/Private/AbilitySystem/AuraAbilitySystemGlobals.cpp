// Copyright Hung


#include "AbilitySystem/AuraAbilitySystemGlobals.h"

#include "AuraGameplayEffectTypes.h"

FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FAuraGameplayEffectContext();
}
