// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "CostCooldownAbility.h"
#include "DamageAbility.generated.h"

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
class AURA_API UDamageAbility : public UCostCooldownAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="Dmg", DefaultToSelf="Ability", HidePin="Ability"))
	static float GetDamageAtLevel(const UDamageAbility* Ability, const int32 Level, UPARAM(meta=(GameplayTagFilter="Damage")) const FGameplayTag TypeTag);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="Default|Damage")
	bool bStagger = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|Damage", meta=(ForceInlineRow, GameplayTagFilter="Damage"))
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	UFUNCTION(BlueprintCallable)
	void CauseDamageToActors(UPARAM(meta=(GameplayTagFilter="GameplayCue.Impact")) const FGameplayTag GameplayCueTag,
		const TArray<AActor*>& Actors, USoundBase* ImpactSound);
	// Call in GC_MeleeImpact
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static void MeleeImpactCueFromEffectContext(const UObject* WorldContextObject, const FGameplayEffectContextHandle& EffectContextHandle);
};
