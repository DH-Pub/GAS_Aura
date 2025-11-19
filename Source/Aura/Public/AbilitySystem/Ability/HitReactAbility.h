// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "HitReactAbility.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UHitReactAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UHitReactAbility();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
