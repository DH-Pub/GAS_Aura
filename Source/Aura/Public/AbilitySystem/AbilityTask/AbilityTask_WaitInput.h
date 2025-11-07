// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitInput.generated.h"

class UAuraGameplayAbility;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpecInputDelegate, float, TimeWaited);
/**
 * Waits until the input is pressed from activating an ability.
 * This should be true immediately upon starting the ability, since the key was pressed to activate it.
 * We expect server to execute this task in parallel and keep its own time. We do not keep track of
 */
UCLASS()
class AURA_API UAbilityTask_WaitSpecInputPressed : public UAbilityTask
{	// Based on UAbilityTask_WaitInputPress
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FSpecInputDelegate OnPress;
	/**
	 * Wait until the user presses the input button for this ability's activation.
	 * @returns time this node spent waiting for the press. 0 if input was already down.
	 */
	UFUNCTION(BlueprintCallable, meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="TRUE"))
	static UAbilityTask_WaitSpecInputPressed* WaitSpecInputPressed(UAuraGameplayAbility* OwningAbility);

	virtual void Activate() override;
	UFUNCTION()
	void OnPressedCallback();

	virtual void OnDestroy(bool bInOwnerFinished) override;
protected:
	float StartTime = 0.f;
	FDelegateHandle DelegateHandle;
};


/**
 * Waits until the input is released from activating an ability.
 * Clients will replicate a 'release input' event to the server, but not the exact time it was held locally.
 * We expect server to execute this task in parallel and keep its own time.
 */
UCLASS()
class AURA_API UAbilityTask_WaitSpecInputReleased : public UAbilityTask
{	// Based on UAbilityTask_WaitInputRelease
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FSpecInputDelegate OnReleased;
	/**
	 * Wait until the user releases the input button for this ability's activation.
	 * @returns time from hitting this node, till release. 0 if input was already released.
	 */
	UFUNCTION(BlueprintCallable, meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="TRUE"))
	static UAbilityTask_WaitSpecInputReleased* WaitSpecInputReleased(UAuraGameplayAbility* OwningAbility);

	virtual void Activate() override;

	UFUNCTION()
	void OnReleasedCallback();

	virtual void OnDestroy(bool bInOwnerFinished) override;
protected:
	float StartTime = 0.f;
	FDelegateHandle DelegateHandle;
};
