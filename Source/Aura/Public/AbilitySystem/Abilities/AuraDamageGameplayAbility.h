// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UAuraDamageGameplayAbility();
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);
	UFUNCTION(BlueprintCallable)
	void CauseDamageToActors(TArray<AActor*> Actors, UPARAM(meta=(GameplayTagFilter="GameplayCue.Impact")) FGameplayTag GameplayCueTag,
		UPARAM(ref) FGameplayCueParameters& GameplayCueParameters, bool bStagger = false);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="Damage", meta=(ForceInlineRow, GameplayTagFilter="Damage"))
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
};
