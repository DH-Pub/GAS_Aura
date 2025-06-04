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
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category="ProjectileAbility")
	void SpawnProjectile(const FVector& ProjectileTargetLocation);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> ProjectileClass;
private:
	UPROPERTY(EditDefaultsOnly)
	bool bStagger = false;
};
