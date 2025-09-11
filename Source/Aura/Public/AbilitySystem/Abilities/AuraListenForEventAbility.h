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
public:
	UAuraListenForEventAbility();
protected:
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	TSubclassOf<UGameplayEffect> EventBasedEffectClass;
	UPROPERTY()
	FGameplayTagContainer ModifiersDataTags; // DataTags from EventBasedEffectClass

	UFUNCTION(BlueprintCallable)
	void ApplyEventEffectsToSelf(const FGameplayEventData& Payload);
};
