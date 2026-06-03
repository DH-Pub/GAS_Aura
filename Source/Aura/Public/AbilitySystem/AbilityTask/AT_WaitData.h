// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_WaitData.generated.h"


USTRUCT(BlueprintType)
struct FGATargetData_CommonData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	FGATargetData_CommonData() {}

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> TargetActor;
	/** Returns all actors targeted, almost always overridden */
	virtual TArray<TWeakObjectPtr<AActor>> GetActors() const override {return {TargetActor};}

	UPROPERTY(BlueprintReadWrite)
	FVector_NetQuantize10 Location;
	UPROPERTY(BlueprintReadWrite)
	FVector_NetQuantizeNormal Direction;
	virtual bool HasOrigin() const override {return true;}
	virtual FTransform GetOrigin() const override
	{
		return FTransform(Direction.ToOrientationQuat(), Location);
	}

	// Required for all child structs of FGameplayAbilityTargetData
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct();}

	// Required for all child structs of FGameplayAbilityTargetData
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << TargetActor;
		Location.NetSerialize(Ar, Map, bOutSuccess);
		Direction.NetSerialize(Ar, Map, bOutSuccess);
		return bOutSuccess;
	}
};
template<>
struct TStructOpsTypeTraits<FGATargetData_CommonData> : public TStructOpsTypeTraitsBase2<FGATargetData_CommonData>
{	// This is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	enum {WithNetSerializer = true};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDataReceivedSignature, const FVector&, Location,
	const FRotator&, Rotation, AActor*, Actor);
/**
 * Send Basic Data to Server
 */
UCLASS()
class AURA_API UAT_WaitData : public UAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FDataReceivedSignature OnDataReceived;

	/**
	 * @param LocTolerance: Server's InLoc's Distance to Client's InLoc is within Tolerance
	 */
	UFUNCTION(BlueprintCallable, Category="Aura|Task", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility",
		BlueprintInternalUseOnly="true"))
	static UAT_WaitData* SendData(class UAuraGameplayAbility* OwningAbility, const FVector& Location,
		const FVector& Direction, AActor* InActor, const float LocTolerance = 1.f);

protected:
	virtual void Activate() override;
private:
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

	UPROPERTY()
	FVector Location;
	UPROPERTY()
	FVector Direction;
	UPROPERTY()
	TWeakObjectPtr<AActor> Actor;

	UPROPERTY()
	float LocationTolerance;

	FDelegateHandle DelegateHandle;
};
