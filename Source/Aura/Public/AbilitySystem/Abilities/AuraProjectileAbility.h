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
protected:
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr) override;

	UFUNCTION(BlueprintCallable, Category="ProjectileAbility")
	void SpawnProjectile(const FVector& TargetLocation, const FVector& InSpawnLocation, bool bStartFromCharacter = true,
		const float SpawnDistance = 60.f, float SpawnHeightAdd = 50.f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> ProjectileClass;
private:
	UPROPERTY(EditDefaultsOnly)
	bool bStagger = false;
};
