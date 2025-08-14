// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

class AAuraPlayerController;
class AAuraCharacterBase;
class IEnemyInterface;
class ICombatInterface;

/**
 * Base GameplayAbility for this project
 * Gameplay Ability is only replicated to the owning player by default
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UAuraGameplayAbility();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// Projects may want to initiate passives or do other "BeginPlay" type of logic here.
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AAuraCharacterBase> AuraCharacterFromActorInfo = nullptr;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AAuraPlayerController> AuraPlayerController = nullptr;
private:
	UPROPERTY(EditDefaultsOnly, Category="Default")
	bool bMaxSpeedZeroedOnActivated = false;
};
