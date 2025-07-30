// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

struct FInstancedStruct;

USTRUCT(BlueprintType)
struct FSummonInfo
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
	int32 NumMinions = 5;

	UPROPERTY(EditDefaultsOnly, Category="Summoning")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(BlueprintReadWrite, Category="Summoning")
	FSummonInfo SummonInfo = FSummonInfo();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Summoning")
	int32 SpawnLocationIndex;

	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0), Category="Summoning")
	float MinSpawnDistance = 150.f;
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0), Category="Summoning")
	float MaxSpawnDistance = 350.f;
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0.f, UIMax= 360.f, Delta=1.f), Category="Summoning")
	float SpawnSpread = 120.f;
	
	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs = "ReturnValue"))
	static bool InstancedStructToSummonInfo(const FInstancedStruct& InstancedStruct, TArray<FVector_NetQuantize>& Locations);

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
