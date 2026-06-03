// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilitySystem/Ability/TargetData/TargetActor_Indicator.h"
#include "AT_WaitIndicatorTrace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitIndicatorDelegate, const FGameplayAbilityTargetDataHandle&, Data);

/**
 * Based on AbilityTask_WaitTargetData
 */
UCLASS()
class AURA_API UAT_WaitIndicatorTrace : public UAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FWaitIndicatorDelegate ValidData;
	UPROPERTY(BlueprintAssignable)
	FWaitIndicatorDelegate Cancelled;

	/**
	 * @param bLocalOnly if false, Actor will be spawned and visible for non-local players
	 * @return
	 */
	UFUNCTION(BlueprintCallable, meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UAT_WaitIndicatorTrace* WaitIndicatorTrace(class UAuraGameplayAbility* OwningAbility,
		const TSubclassOf<ATargetActor_Indicator> Class, const FIndicatorDetails Details, const bool bLocalOnly = true);

	/**
	 * To show "ExposeOnSpawn" actor properties.
	 * Read AbilityTask.h -> BeginSpawningActor() / FinishSpawningActor() for more details
	 */
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Abilities")
	virtual bool BeginSpawningActor(UAuraGameplayAbility* OwningAbility, TSubclassOf<ATargetActor_Indicator> Class, ATargetActor_Indicator*& SpawnedActor);
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Abilities")
	virtual void FinishSpawningActor(UAuraGameplayAbility* OwningAbility, ATargetActor_Indicator* SpawnedActor);

	void InitIndicatorActor(ATargetActor_Indicator* Indicator);
public:
	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool bInOwnerFinished) override;
protected:
	virtual void Activate() override;

	UFUNCTION()
	void OnConfirmCallback();
	UFUNCTION()
	void OnCancelCallback();

	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);
	void OnTargetDataReplicatedCancelledCallback();

	UPROPERTY()
	TObjectPtr<UAuraGameplayAbility> AuraAbility;
	UPROPERTY()
	TObjectPtr<class AAuraPlayerController> AuraPC;

	UPROPERTY()
	TSubclassOf<ATargetActor_Indicator> IndicatorClass;
	UPROPERTY()
	TObjectPtr<ATargetActor_Indicator> IndicatorActor; /** The TargetActor that we spawned */

	FIndicatorDetails Details;

	bool bLocalOnly;

	FVector VecFromCharacter; // To keep Indicator same distance from Character
};
