// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "InputTriggers.h"
#include "AuraInputAbility.generated.h"

class UAuraInputComponent;
/**
 * Input connected to this ability HAS to be NON-OneShot Hold Trigger (UInputTriggerHold) And InstancedPerActor
 * Base class for Ability that use input
 * Check "class UGameplayAbility_CharacterJump : public UGameplayAbility"
 */
UCLASS()
class AURA_API UAuraInputAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UAuraInputAbility();
	// Input =======================================================================================================
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	// This is Added to GetDynamicSpecSourceTags() during AddCharacterAbilities()
	UPROPERTY(EditDefaultsOnly, Category="Default", meta=(GameplayTagFilter="Input"))
	FGameplayTag StartupInputTag;
	UPROPERTY()
	TObjectPtr<UInputAction> InputAction;
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintNativeEvent)
	void StartPressedOngoing(); // Started / Ongoing start
	UFUNCTION(BlueprintNativeEvent)
	void StartHoldTriggered(); // Triggered start

	UFUNCTION(BlueprintNativeEvent)
	void TapReleased(); // Canceled
	UFUNCTION(BlueprintNativeEvent)
	void HoldReleased(); // Completed
	
	UFUNCTION(BlueprintNativeEvent)
	void DoubleClick(); // override this
	UFUNCTION(BlueprintNativeEvent)
	void TripleClick(); // override this
private:
	ETriggerEvent AbilityTriggerEvent = ETriggerEvent::None;
	
	UPROPERTY(EditDefaultsOnly, Category="Default")
	float RepeatDelayTime = 0.5;
	FTimerHandle RepeatDelayTimer; // for (RepeatedTap)
	UPROPERTY(EditDefaultsOnly, Category="Default")
	uint8 MaxRepeatedClick = 3;
	uint8 ClickNums = 0;
public:
	void SetAbilityTriggerEvent(ETriggerEvent TriggerEvent); // Called in AbilitySystemComponent

	FInputActionValue GetBoundAuraActionValue();
};
