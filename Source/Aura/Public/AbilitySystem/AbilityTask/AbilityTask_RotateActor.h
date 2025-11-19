// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_RotateActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFinishDelegate, float, RotateTime);
/**
 * TODO: This is not being used
 */
UCLASS()
class AURA_API UAbilityTask_RotateActor : public UAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FFinishDelegate OnFinish;
	/**
	 * Wait until actor rotate to aim direction before continue
	 * @returns time this node spent waiting for the press. 0 if input was already down.
	 */
	UFUNCTION(BlueprintCallable, meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="TRUE"))
	static UAbilityTask_RotateActor* WaitSpecInputPressed(class UAuraGameplayAbility* OwningAbility);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
protected:
	UPROPERTY()
	TObjectPtr<class AAuraCharacterBase> AuraCharacter;
	float StartTime = 0.f;
};
