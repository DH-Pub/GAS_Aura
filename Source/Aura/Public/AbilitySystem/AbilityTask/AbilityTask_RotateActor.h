// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_RotateActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FFinishRotateDelegate, const FVector&, Direction, AActor*, TargetActor,
	float, RotateTime);
/**
 * TODO: This is not being used
 */
UCLASS()
class AURA_API UAbilityTask_RotateActor : public UAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FFinishRotateDelegate OnFinish;
	/**
	 * Wait until actor rotate to aim direction before continue
	 * @returns time this node spent waiting for the press. 0 if input was already down.
	 */
	UFUNCTION(BlueprintCallable, meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="TRUE"))
	static UAbilityTask_RotateActor* WaitRotateToTarget(class UAuraGameplayAbility* OwningAbility,
		const float Multiplier = 2.f);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
protected:
	UPROPERTY()
	TObjectPtr<class AAuraCharacterBase> AuraCharacter;

	FVector_NetQuantizeNormal Direction;
	FRotator TargetRotation;

	UPROPERTY()
	TObjectPtr<AActor> TargetActor;

	FRotator RotationRate;
	float Multiplier = 1.f;
	float StartTime = 0.f;
private:
	float GetAxisDeltaRot(const float InRotationRate, const float DeltaTime)
	{
		return InRotationRate >= 0.f ? FMath::Min(InRotationRate * DeltaTime, 360.f) : 360.f;
	}

	UFUNCTION()
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
	FDelegateHandle DelegateHandle;
};
