// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraInputAbility.generated.h"

/**
 * Base class for Ability that use input
 * Check "class UGameplayAbility_CharacterJump : public UGameplayAbility"
 */
UCLASS()
class AURA_API UAuraInputAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	// Input =======================================================================================================
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	// This is Added to GetDynamicSpecSourceTags() during AddCharacterAbilities()
	UPROPERTY(EditDefaultsOnly, Category="Default", meta=(GameplayTagFilter="Input"))
	FGameplayTag StartupInputTag;
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UPROPERTY(BlueprintReadWrite)
	FTimerHandle HoldInputTimer;
	virtual void HoldThresholdReached();
	bool bPassHoldThreshold = false;
	
	UPROPERTY(BlueprintReadWrite)
	FTimerHandle MultiClickTimer;
	virtual void MultiClickTimePassed();
	bool bPassMultiClickThreshold = false;

	virtual void HoldReleased(){}; // override this
	virtual void TapReleased(){}; // override this
	virtual void DoubleClick(){}; // override this
	virtual void TripleClick(){}; // override this
private:
	UPROPERTY(VisibleAnywhere, Category="Default")
	uint8 ClickNums = 0;
	UPROPERTY(EditDefaultsOnly, Category="Default")
	float HoldThreshold = 0.35f;
	UPROPERTY(EditDefaultsOnly, Category="Default")
	float MultiClickThreshold = 0.5f;
	
	FVector2D ClickScreenPosition = FVector2D::ZeroVector;
	UPROPERTY(EditDefaultsOnly, Category="Default|Mouse")
	float MouseMoveLimit = 50.f;
};
