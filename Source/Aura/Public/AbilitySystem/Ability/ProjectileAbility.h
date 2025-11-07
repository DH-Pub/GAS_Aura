// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "DamageAbility.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "ProjectileAbility.generated.h"

enum class ECombatSocket : uint8;
class AAuraProjectile;
/**
 * 
 */
UCLASS()
class AURA_API UProjectileAbility : public UDamageAbility
{
	GENERATED_BODY()
public:
	void SpawnProjectile(FVector& SpawnLoc, const FVector& CursorHit, FRotator InRot, const AActor* HomingTarget,
		float HeightAdd = 50.f);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|Projectile")
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|Projectile")
	FScalableFloat ProjectileNums = 1.f;
	// const func creates Target[self], use static
	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="Projectiles", HidePin="Ability", DefaultToSelf="Ability"))
	static int32 GetProjectileNumsAtLevel(const UProjectileAbility* Ability, const int32 Level)
	{return Ability->ProjectileNums.GetValueAtLevel(Level);}

private:
	UPROPERTY(EditDefaultsOnly, Category="Default|Projectile")
	float ProjectileSpread = 90.f;
	UPROPERTY(EditDefaultsOnly, Category="Default|Projectile")
	float ProjectilePitch = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Default|Projectile", meta=(InlineEditConditionToggle))
	bool bHoming = false;
	UPROPERTY(EditDefaultsOnly, Category="Default|Projectile", meta=(EditCondition="bHoming"))
	float HomingAcceleration = 1000.f;
};


/*
 * ==================================== Ability Task ===========================================================
 */
USTRUCT(BlueprintType)
struct FGA_TargetData_ProjectileInfo : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	FGA_TargetData_ProjectileInfo() {}

	UPROPERTY()
	FVector Location = FVector();
	UPROPERTY()
	FRotator Rotation = FRotator();

	float ActivationTime = 0.f;
	virtual FTransform GetOrigin() const override // Use Origin for spawn point
	{
		FTransform Origin(Location);
		Origin.SetRotation(Rotation.Quaternion());
		Origin.SetScale3D(FVector(ActivationTime, 0.f, 0.f));
		return Origin;
	}

	UPROPERTY()
	FVector EndPoint = FVector();
	virtual bool HasEndPoint() const override {return true;}
	virtual FVector GetEndPoint() const override {return EndPoint;}
	// Required for all child structs of FGameplayAbilityTargetData
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct();}

	// Required for all child structs of FGameplayAbilityTargetData
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << ActivationTime;
		Location.NetSerialize(Ar, Map, bOutSuccess);
		Rotation.NetSerialize(Ar, Map, bOutSuccess);
		EndPoint.NetSerialize(Ar, Map, bOutSuccess);
		return true;
	}
};
template<>
struct TStructOpsTypeTraits<FGA_TargetData_ProjectileInfo> : TStructOpsTypeTraitsBase2<FGA_TargetData_ProjectileInfo>
{
	enum
	{
		WithNetSerializer = true // This is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFinishSpawn/*, const FGameplayAbilityTargetDataHandle&, Data*/);
/*
 * 
 */
UCLASS()
class AURA_API UAbilityTask_SpawnProjectile : public UAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FFinishSpawn OnSpawnFinish;

	UFUNCTION(BlueprintCallable, Category = "Ability|Task", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility",
		BlueprintInternalUseOnly="true"))
	static UAbilityTask_SpawnProjectile* SpawnProjectile(UProjectileAbility* OwningAbility,
		const ECombatSocket SpawnSocket, FVector EndPoint, AActor* Target, float HeightIfHitGround = 60.f);
protected:
	virtual void Activate() override;

	UPROPERTY()
	TObjectPtr<UProjectileAbility> ProjectileAbility;
	FVector SocketLocation;
	FVector EndPoint; // Player: Cursor Hit, Bot: Target Loc
	UPROPERTY()
	TObjectPtr<AActor> Target;
	float HeightIfHitGround; // AddedSpawnHeight in case projectile hit ground on spawn
private:
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

	FDelegateHandle DelegateHandle;
};
