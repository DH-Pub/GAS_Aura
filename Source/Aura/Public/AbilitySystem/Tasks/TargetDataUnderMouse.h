// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);
/**
 *  Used in GameplayAbility BP (ex: GA_FireBolt)
 *  Return TargetData containing mouse FHitResult
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Task", meta=(DisplayName="TargetDataUnderMouse", HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;
private:
	virtual void Activate() override;
	void SendMouseCursorData() const;

	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,  FGameplayTag ActivationTag) const;
};
