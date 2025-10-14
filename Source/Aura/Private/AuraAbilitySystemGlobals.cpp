// Copyright Hung


#include "AuraAbilitySystemGlobals.h"

#include "AuraEffectTypes.h"
#include "Character/AuraCharacterBase.h"

UAuraAbilitySystemComponent* UAuraAbilitySystemGlobals::GetAuraASC(const AActor* Actor)
{
	if (const AAuraCharacterBase* AuraChara = Cast<AAuraCharacterBase>(Actor))
	{
		return AuraChara->GetAuraAbilitySystemComponent();
	}
	return nullptr;
}

FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FAuraEffectContext();
}
