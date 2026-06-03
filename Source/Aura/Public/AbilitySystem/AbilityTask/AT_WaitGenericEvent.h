// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_WaitGenericEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventDelegate, float, TimeWaited);
/**
 * Based on UAbilityTask_WaitInputPress / UAbilityTask_WaitInputRelease
 * Waits until receiving a EAbilityGenericReplicatedEvent.
 * Clients will replicate a 'release input' event to the server, but not the exact time it was held locally.
 * We expect server to execute this task in parallel and keep its own time.
 */
UCLASS()
class AURA_API UAT_WaitGenericEvent : public UAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FEventDelegate OnReceived;
	/**
	 * Wait until the user presses the input button for this ability's activation.
	 * @returns time this node spent waiting for the press. 0 if input was already down.
	 */
	UFUNCTION(BlueprintCallable, meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="TRUE"))
	static UAT_WaitGenericEvent* WaitGenericReplicatedEvent(class UAuraGameplayAbility* OwningAbility,
		EAbilityGenericReplicatedEvent::Type Event);

	virtual void OnDestroy(bool bInOwnerFinished) override;

protected:
	virtual void Activate() override;


	UFUNCTION()
	void OnCallback();
	EAbilityGenericReplicatedEvent::Type Event;
	float StartTime = 0.f;
};
