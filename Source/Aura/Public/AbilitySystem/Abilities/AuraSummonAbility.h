// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

class UNiagaraSystem;
class AAuraCharacterBase;
struct FInstancedStruct;

/*
 * GameplayCue can only send max 2 per frame with default settings, that's why we use this and convert it to FInstancedStruct
 */
USTRUCT(BlueprintType)
struct FSummonCueInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector_NetQuantize> Locations;
};
/**
 * 
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetSpawnLocations();
	UFUNCTION(BlueprintPure)
	TSubclassOf<APawn> GetRandomMinionsClass() {return MinionClasses[FMath::RandRange(0, MinionClasses.Num() - 1)];}

	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0), Category="Summoning")
	int32 NumMinions = 5; // How many to summon when ActivateAbility

	UPROPERTY(EditDefaultsOnly, Category="Summoning")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(BlueprintReadWrite, Category="Summoning")
	FSummonCueInfo SummonInfo = FSummonCueInfo();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Summoning")
	int32 SpawnLocationIndex;

	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0), Category="Summoning")
	float MinSpawnDistance = 150.f;
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0), Category="Summoning")
	float MaxSpawnDistance = 350.f;
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0.f, UIMax= 360.f, Delta=1.f), Category="Summoning")
	float SpawnSpread = 120.f;
protected:
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"))
	bool SpawnEnemiesByLocations();

	// Call this in GC_Summon
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static void SummonCueFromEffectContext(const UObject* WorldContextObject, const FGameplayEffectContextHandle& EffectContextHandle,
		UNiagaraSystem* Effect);
	/*UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs = "ReturnValue"))
	static bool EffectContextHandleToSummonInfo(const FGameplayEffectContextHandle& EffectContextHandle, TArray<FVector_NetQuantize>& Locations);*/
};
