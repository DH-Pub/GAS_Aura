// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "DamageAbility.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "ProjectileAbility.generated.h"

enum class ECombatSocket : uint8;

/**
 *
 */
UCLASS()
class AURA_API UProjectileAbility : public UDamageAbility
{
	GENERATED_BODY()
public:
	void SpawnProjectile(FVector& SpawnLoc, FRotator InRot, const AActor* HomingTarget, float HeightAdd = 50.f);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Projectile")
	TSubclassOf<class AAuraProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Projectile")
	FScalableFloat ProjectileNums = 1.f;
	/*// const func creates Target[self], use static
	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="Projectiles", HidePin="Ability", DefaultToSelf="Ability"))
	static int32 GetProjectileNumsAtLevel(const UProjectileAbility* Ability, const int32 Level)
	{return Ability->ProjectileNums.GetValueAtLevel(Level);}*/

private:
	UPROPERTY(EditDefaultsOnly, Category="Aura|Projectile")
	float ProjectileSpread = 90.f;
	UPROPERTY(EditDefaultsOnly, Category="Aura|Projectile")
	float ProjectilePitch = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Projectile", meta=(InlineEditConditionToggle))
	bool bHoming = false;
	UPROPERTY(EditDefaultsOnly, Category="Aura|Projectile", meta=(EditCondition="bHoming"))
	float HomingAcceleration = 1000.f;

public:
	virtual void GetAbilityDetails(FAbilityDetails& Details) const override;
};


/*
 * ===================================== ProjectileInfo ======================================================
 */
USTRUCT()
struct FGATargetData_ProjectileInfo : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	FGATargetData_ProjectileInfo() {}

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

	// Required for all child structs of FGameplayAbilityTargetData
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct();}
	// Required for all child structs of FGameplayAbilityTargetData
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << ActivationTime;
		Location.NetSerialize(Ar, Map, bOutSuccess);
		Rotation.NetSerialize(Ar, Map, bOutSuccess);
		return true;
	}
};
template<>
struct TStructOpsTypeTraits<FGATargetData_ProjectileInfo> : TStructOpsTypeTraitsBase2<FGATargetData_ProjectileInfo>
{	// REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	enum {WithNetSerializer = true};
};
/*
 * ==================================== Ability Task ===========================================================
 */
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
		const ECombatSocket SpawnSocket, const FVector& Direction, AActor* Target, float HeightIfHitGround = 60.f);
protected:
	virtual void Activate() override;

	UPROPERTY()
	TObjectPtr<UProjectileAbility> ProjectileAbility;
	UPROPERTY()
	TObjectPtr<AAuraCharacterBase> AuraCharacter;
	FVector SocketLocation;
	UPROPERTY()
	TObjectPtr<AActor> Target;
	float HeightIfHitGround; // AddedSpawnHeight in case projectile hit ground on spawn
private:
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

	FDelegateHandle DelegateHandle;

	FVector Direction;
};
