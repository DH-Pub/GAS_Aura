// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

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
	
	UFUNCTION(BlueprintImplementableEvent)
	FText GetDescription(const int32 Level, const int32 AddLevel); // Override this in each ability's BP
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// Projects may want to initiate passives or do other "BeginPlay" type of logic here.
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	FText AbilityName;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AAuraCharacterBase> AuraCharacterFromActorInfo = nullptr;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AAuraPlayerController> AuraPlayerController = nullptr;

	FGameplayTagContainer& AddGenericAssetTags(FGameplayTagContainer& Tags);
	void SetGenericCancelBlockAbility();
private:
	UPROPERTY(EditDefaultsOnly, Category="Default")
	bool bMaxSpeedZeroedOnActivated = false; //TODO: Make movement ability for Enemy and replace this
};
