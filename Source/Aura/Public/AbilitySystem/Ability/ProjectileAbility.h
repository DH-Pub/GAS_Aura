// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "DamageAbility.h"
#include "ProjectileAbility.generated.h"

class AAuraProjectile;
/**
 * 
 */
UCLASS()
class AURA_API UProjectileAbility : public UDamageAbility
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(const FVector& InSpawnLocation, AActor* MovingTarget, bool bStartFromCharacter = true,
		const float SpawnDistance = 60.f, float SpawnHeightAdd = 50.f, float Pitch = 0.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|Projectile")
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|Projectile")
	FScalableFloat ProjectileNums;
	// const func creates Target[self], use static
	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="Projectiles", HidePin="Ability", DefaultToSelf="Ability"))
	static int32 GetProjectileNumsAtLevel(const UProjectileAbility* Ability, const int32 Level)
	{return Ability->ProjectileNums.GetValueAtLevel(Level);}

	UPROPERTY(EditDefaultsOnly, Category="Default|Projectile")
	float ProjectileSpread = 90.f;
};
