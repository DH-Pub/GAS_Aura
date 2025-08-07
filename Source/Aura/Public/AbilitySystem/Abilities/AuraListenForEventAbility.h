// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraListenForEventAbility.generated.h"

/**
 * This Ability use WaitGameplayEvent in blueprint to get payload
 * Ability only need to activate once and wait for event; EndAbility is NOT used
 */
UCLASS()
class AURA_API UAuraListenForEventAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
protected:
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> EventBasedEffectClass;
	UPROPERTY()
	FGameplayTagContainer ModifiersDataTags; // DataTags from EventBasedEffectClass

	UFUNCTION(BlueprintCallable)
	void ApplyEventEffectsToSelf(const FGameplayEventData& Payload);
};
