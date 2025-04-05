// Copyright Hung


#include "Input/AuraInputConfig.h"

UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag) const
{
	return AbilityInputActions.FindRef(InputTag);
}
