// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_AimData.generated.h"

USTRUCT(BlueprintType)
struct FGameplayAbilityTargetData_AimData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	FGameplayAbilityTargetData_AimData() {}

	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActor;
	/** Returns all actors targeted, almost always overridden */
	virtual TArray<TWeakObjectPtr<AActor>> GetActors() const override {return {TargetActor};}

	UPROPERTY()
	FVector_NetQuantizeNormal AimDirection = FVector::ZeroVector;
	float ActivatedTime = 0.f;
	virtual FTransform GetOrigin() const override
	{
		FTransform Result(AimDirection);
		Result.SetScale3D(FVector(ActivatedTime, 0.f, 0.f)); // store float in Scale
		return Result;
	}

	bool bHasEndPoint = false;
	UPROPERTY()
	FVector EndPoint = FVector(); // Cursor hit for player, CombatActor Loc for Bot
	virtual bool HasEndPoint() const override { return bHasEndPoint; }
	virtual FVector GetEndPoint() const override {return EndPoint;}

	// Required for all child structs of FGameplayAbilityTargetData
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct();}

	// Required for all child structs of FGameplayAbilityTargetData
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << bHasEndPoint;
		Ar << ActivatedTime;
		Ar << TargetActor;
		AimDirection.NetSerialize(Ar, Map, bOutSuccess);
		EndPoint.NetSerialize(Ar, Map, bOutSuccess);
		return true;
	}
};
template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_AimData> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_AimData>
{	// This is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	enum {WithNetSerializer = true};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMouseTargetDataSignature/*, const FGameplayAbilityTargetDataHandle&, Data*/,
	const FVector&, Direction, const FVector&, CursorHit, AActor*, Target);
/**
 *	Send Input Data to remote (Client -> Server)
 *  Used in GameplayAbility BP (ex: GA_FireBolt)
 *  Return TargetData containing mouse FHitResult
 */
UCLASS()
class AURA_API UAbilityTask_AimData : public UAbilityTask
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category="Ability|Task", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility",
		BlueprintInternalUseOnly="true"))
	static UAbilityTask_AimData* SendAimData(UGameplayAbility* OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;
protected:
	virtual void Activate() override;
private:
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

	FDelegateHandle DelegateHandle;
};
