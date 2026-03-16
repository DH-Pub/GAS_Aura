// Copyright Hung


#include "AbilitySystem/Ability/AuraPassiveAbility.h"

void UAuraPassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

}

void UAuraPassiveAbility::ReceiveDeactivate(const FGameplayTag& AbilityTag)
{
	if (GetAssetTags().HasTagExact(AbilityTag)) K2_EndAbility();
}
