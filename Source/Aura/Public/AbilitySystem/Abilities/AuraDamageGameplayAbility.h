// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "CostCooldownAbility.h"
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


USTRUCT(BlueprintType)
struct FAbilityDescriptionDetails // For Ability Descriptions
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	int32 Level = 1;
	UPROPERTY(BlueprintReadOnly)
	int32 LevelChanged = 1;

	UPROPERTY(BlueprintReadOnly)
	float Damage = 0.f;
	UPROPERTY(BlueprintReadOnly)
	float DamageChanged = 0.f;
	
	UPROPERTY(BlueprintReadOnly)
	float CostMana = 0.f;
	UPROPERTY(BlueprintReadOnly)
	float CostManaChanged = 0.f;
	UPROPERTY(BlueprintReadOnly)
	float CostHealth = 0.f;
	UPROPERTY(BlueprintReadOnly)
	float CostHealthChanged = 0.f;
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UCostCooldownAbility
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default", meta=(ForceInlineRow, GameplayTagFilter="Damage"))
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
	UFUNCTION(BlueprintPure)
	float GetDamageAtLevel(const int32 Level, UPARAM(meta=(GameplayTagFilter="Damage")) const FGameplayTag TypeTag);
	UFUNCTION(BlueprintCallable)
	void GetDamageAtLevelChanged(float& Damage, float& DamageChanged,
		UPARAM(meta=(GameplayTagFilter="Damage")) const FGameplayTag TypeTag,
		const int32 Level = 0, const int32 LevelDelta = 0);

	UFUNCTION(BlueprintCallable)
	void CauseDamageToActors(UPARAM(meta=(GameplayTagFilter="GameplayCue.Impact")) const FGameplayTag GameplayCueTag,
		const TArray<AActor*>& Actors, USoundBase* ImpactSound, bool bStagger = false);
	// Call in GC_MeleeImpact
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static void MeleeImpactCueFromEffectContext(const UObject* WorldContextObject, const FGameplayEffectContextHandle& EffectContextHandle);
};
