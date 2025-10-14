// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_AimData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, Data);

USTRUCT(BlueprintType)
struct FGameplayAbilityTargetData_AimData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	FGameplayAbilityTargetData_AimData() {}

	/** Returns all actors targeted, almost always overridden */
	virtual TArray<TWeakObjectPtr<AActor>> GetActors() const
	{
		return TArray<TWeakObjectPtr<AActor>>();
	}

	UPROPERTY()
	FVector AimDirection = FVector();
	virtual bool HasEndPoint() const override { return true; }
	virtual FVector GetEndPoint() const override {return AimDirection;}

	// Required for all child structs of FGameplayAbilityTargetData
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct();}
	
	// Required for all child structs of FGameplayAbilityTargetData
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		AimDirection.NetSerialize(Ar, Map, bOutSuccess);
		return true;
	}
};
template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_AimData> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_AimData>
{
	enum
	{
		WithNetSerializer = true // This is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};
/**
 *	Send Input Data to remote (Client -> Server)
 *  Used in GameplayAbility BP (ex: GA_FireBolt)
 *  Return TargetData containing mouse FHitResult
 */
UCLASS()
class AURA_API UAbilityTask_AimData : public UAbilityTask
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Task", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility",
		BlueprintInternalUseOnly="true"))
	static UAbilityTask_AimData* SendAimData(UGameplayAbility* OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;
protected:
	virtual void Activate() override;
private:
	void SendMouseCursorData() const;
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag) const;
};
