// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

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
	TSubclassOf<APawn> GetRandomMinionsClass();

	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0), Category="Summoning")
	int32 NumMinions = 5;

	UPROPERTY(EditDefaultsOnly, Category="Summoning")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(BlueprintReadWrite, Category="Summoning")
	TArray<FVector_NetQuantize> SpawnLocations;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Summoning")
	int32 SpawnLocationIndex;

	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0), Category="Summoning")
	float MinSpawnDistance = 150.f;
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0), Category="Summoning")
	float MaxSpawnDistance = 350.f;
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0.f, UIMax= 360.f, Delta=1.f), Category="Summoning")
	float SpawnSpread = 120.f;
};
