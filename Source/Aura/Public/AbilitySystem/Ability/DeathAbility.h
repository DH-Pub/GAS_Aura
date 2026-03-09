// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "DeathAbility.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UDeathAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UDeathAbility();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

public:
	static void KillAllSummons(AAuraCharacterBase* Chara);
};
