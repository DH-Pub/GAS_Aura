// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "AuraProjectileAbility.generated.h"

class AAuraProjectile;
/**
 * 
 */
UCLASS()
class AURA_API UAuraProjectileAbility : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
public:
	UAuraProjectileAbility();
protected:
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr) override;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(const FVector& TargetLocation, const FVector& InSpawnLocation, bool bStartFromCharacter = true,
		const float SpawnDistance = 60.f, float SpawnHeightAdd = 50.f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|Projectile")
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|Projectile")
	FScalableFloat ProjectileNums;
	UFUNCTION(BlueprintPure)
	int32 GetProjectileNumsAtLevel(const int32 Level) {return ProjectileNums.GetValueAtLevel(Level);}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	int32 NumProjectiles = 3;
private:
	UPROPERTY(EditDefaultsOnly, Category="Default|Projectile")
	bool bStagger = false;
};
