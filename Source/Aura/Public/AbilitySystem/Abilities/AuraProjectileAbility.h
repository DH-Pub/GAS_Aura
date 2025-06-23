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
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category="ProjectileAbility")
	void SpawnProjectile(const FVector& ProjectileTargetLocation, UPARAM(meta=(GameplayTagFilter="CombatSocket")) FGameplayTag SocketTag,
		bool bStartFromCharacter = true, float SpawnDistance = 60.f, float SpawnHeightAdd = 50.f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> ProjectileClass;
private:
	UPROPERTY(EditDefaultsOnly)
	bool bStagger = false;
};
