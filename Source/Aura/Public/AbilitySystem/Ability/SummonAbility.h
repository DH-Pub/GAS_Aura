// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "CostCooldownAbility.h"
#include "SummonAbility.generated.h"

class UNiagaraSystem;
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
class AURA_API USummonAbility : public UCostCooldownAbility
{
	GENERATED_BODY()
public:
	USummonAbility();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	UFUNCTION(BlueprintCallable)
	void SetSpawnLocations();
	UFUNCTION(BlueprintPure)
	TSubclassOf<APawn> GetRandomMinionsClass() {return MinionClasses[FMath::RandRange(0, MinionClasses.Num() - 1)];}
	
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0), Category="Default|Summoning")
	int32 NumMinions = 5; // How many to summon when ActivateAbility
	
	UPROPERTY(EditDefaultsOnly, Category="Default|Summoning")
	TArray<TSubclassOf<APawn>> MinionClasses;
	
	UPROPERTY(BlueprintReadWrite, Category="Default|Summoning")
	FSummonCueInfo SummonInfo = FSummonCueInfo();
	UPROPERTY(BlueprintReadWrite, Category="Default|Summoning")
	int32 SpawnLocationIndex = 0;
	
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0, Delta=1.f), Category="Default|Summoning")
	float MinSpawnDistance = 150.f;
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0, Delta=1.f), Category="Default|Summoning")
	float MaxSpawnDistance = 350.f;
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0.f, UIMax= 360.f, Delta=1.f), Category="Default|Summoning")
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
	UFUNCTION(BlueprintCallable)
	static void SummonCueFromEffectContext(const FGameplayCueParameters& Parameters, UNiagaraSystem* Effect);
	/*UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs = "ReturnValue"))
	static bool EffectContextHandleToSummonInfo(const FGameplayEffectContextHandle& EffectContextHandle, TArray<FVector_NetQuantize>& Locations);*/
};
