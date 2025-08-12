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

	FDamageCue(){}
	FDamageCue(const FVector_NetQuantize& Loc, USoundBase* Sound, UNiagaraSystem* System)
		: Location(Loc), EffectSound(Sound), NiagaraSystem(System){}
	
	UPROPERTY(BlueprintReadOnly)
	FVector_NetQuantize Location = FVector_NetQuantize();
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USoundBase> EffectSound;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> NiagaraSystem;
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
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="Default", meta=(ForceInlineRow, GameplayTagFilter="Damage"))
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	UFUNCTION(BlueprintCallable)
	void CauseDamageToActors(UPARAM(meta=(GameplayTagFilter="GameplayCue.Impact")) FGameplayTag GameplayCueTag,
		const TArray<AActor*>& Actors, USoundBase* ImpactSound, bool bStagger = false);
	// Call in GC_MeleeImpact
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static void MeleeImpactCueFromEffectContext(const UObject* WorldContextObject, const FGameplayEffectContextHandle& EffectContextHandle);
};
