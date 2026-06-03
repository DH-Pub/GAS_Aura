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
	void SpawnProjectile(FVector& SpawnLoc, FRotator& InRot, const AActor* HomingTarget, float HeightAdd = 50.f);

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<class AAuraProjectile>> ProjectilesSpawned;
	UFUNCTION()
	void OnProjectileDestroyed(AActor* DestroyedActor);
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnProjectileDestroyed(AAuraProjectile* DestroyedProjectile);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Projectile")
	FScalableFloat ProjectileNums = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Projectile")
	float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Projectile")
	float MaxTravelDistance = -1.f; // < 0: Infinite
	/**
	 * Allow piercing. <= 0: Infinite
	 * - In FPS, This can be a float called MaxPiercing: Armor/Wall with hardness float value and damage reduced after piercing
	 */
	UPROPERTY(EditDefaultsOnly, Category="Aura|Projectile")
	int32 MaxHitCount = 1;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Projectile", meta=(InlineEditConditionToggle))
	bool bHoming = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Projectile", meta=(EditCondition="bHoming"))
	float HomingAcceleration = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Projectile")
	float ProjectilePitch = 0.f;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Projectile")
	TSubclassOf<class AAuraProjectile> ProjectileClass;

	/*// const func creates Target[self], use static
	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="Projectiles", HidePin="Ability", DefaultToSelf="Ability"))
	static int32 GetProjectileNumsAtLevel(const UProjectileAbility* Ability, const int32 Level)
	{return Ability->ProjectileNums.GetValueAtLevel(Level);}*/
public:
	virtual void GetAbilityDetails(FAbilityDetails& Details) const override;
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

	UFUNCTION(BlueprintCallable, Category="Ability|Task", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility",
		BlueprintInternalUseOnly="true"))
	static UAbilityTask_SpawnProjectile* SpawnProjectile(UProjectileAbility* OwningAbility,
		const ECombatSocket SpawnSocket, const FVector& Direction, AActor* Target, float HeightIfHitGround = 60.f);
protected:
	virtual void Activate() override;

	UPROPERTY()
	TObjectPtr<UProjectileAbility> ProjectileAbility;
	UPROPERTY()
	TObjectPtr<AAuraCharacterBase> AuraCharacter;
	UPROPERTY()
	TObjectPtr<AActor> Target;
	float HeightIfHitGround; // AddedSpawnHeight in case projectile hit ground on spawn
private:
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

	FDelegateHandle DelegateHandle;

	FVector Location;
	FVector Direction;
};
