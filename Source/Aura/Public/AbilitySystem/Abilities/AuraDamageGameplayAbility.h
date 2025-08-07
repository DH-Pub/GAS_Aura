// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FDamageCue
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USoundBase> EffectSound;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> NiagaraSystem;
	
	UPROPERTY(BlueprintReadOnly)
	FVector_NetQuantize Location;
};

USTRUCT(BlueprintType)
struct FDamageCueList
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TArray<FDamageCue> DamageCues;
};
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
	void CauseDamageToActors(UPARAM(meta=(GameplayTagFilter="GameplayCue.Impact")) FGameplayTag GameplayCueTag,
		const TArray<AActor*>& Actors, USoundBase* ImpactSound, bool bStagger = false);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="Damage", meta=(ForceInlineRow, GameplayTagFilter="Damage"))
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	// Call in GC_MeleeImpact
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static void MeleeImpactCueFromEffectContext(const UObject* WorldContextObject, const FGameplayEffectContextHandle& EffectContextHandle);
};
