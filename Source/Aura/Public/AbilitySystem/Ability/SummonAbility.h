// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "CostCooldownAbility.h"
#include "SummonAbility.generated.h"

class UNiagaraSystem;

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

	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0), Category="Aura|Summoning")
	int32 NumMinions = 5; // How many to summon when ActivateAbility

	UPROPERTY(EditDefaultsOnly, Category="Aura|Summoning")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(BlueprintReadWrite, Category="Aura|Summoning")
	TArray<FVector> SummonLocations;
	UPROPERTY(BlueprintReadWrite, Category="Aura|Summoning")
	int32 SpawnLocationIndex = 0;

	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0, Delta=1.f), Category="Aura|Summoning")
	float MinSpawnDistance = 150.f;
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0, Delta=1.f), Category="Aura|Summoning")
	float MaxSpawnDistance = 350.f;
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0.f, UIMax= 360.f, Delta=1.f), Category="Aura|Summoning")
	float SpawnSpread = 120.f;
protected:
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"))
	bool SpawnEnemiesByLocations();
};
