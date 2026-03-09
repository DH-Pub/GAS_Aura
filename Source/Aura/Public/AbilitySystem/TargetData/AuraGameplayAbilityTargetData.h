// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "AuraGameplayAbilityTargetData.generated.h"

// Store general GameplayAbilityTargetData's in this file

/*
 * General send direction
 */
USTRUCT(BlueprintType)
struct FGATargetData_Direction : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	FGATargetData_Direction() {}

	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActor;
	/** Returns all actors targeted, almost always overridden */
	virtual TArray<TWeakObjectPtr<AActor>> GetActors() const override {return {TargetActor};}

	UPROPERTY()
	FVector_NetQuantizeNormal Direction = FVector::ZeroVector;
	virtual FVector GetEndPoint() const override {return Direction;}

	// Required for all child structs of FGameplayAbilityTargetData
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct();}
	// Required for all child structs of FGameplayAbilityTargetData
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << TargetActor;
		Direction.NetSerialize(Ar, Map, bOutSuccess);
		return true;
	}
};
template<>
struct TStructOpsTypeTraits<FGATargetData_Direction> : TStructOpsTypeTraitsBase2<FGATargetData_Direction>
{	// This is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	enum {WithNetSerializer = true};
};
