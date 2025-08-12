// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraInputAbility.h"
#include "MoveMouseAbility.generated.h"

class AAuraPlayerController;
/**
 * 
 */
UCLASS()
class AURA_API UMoveMouseAbility : public UAuraInputAbility
{
	GENERATED_BODY()
public:
	UMoveMouseAbility();
	/*virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;*/

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void HoldReleased() override;
	virtual void TapReleased() override;
	virtual void DoubleClick() override;
	virtual void TripleClick() override;
private:
	UPROPERTY(EditDefaultsOnly, Category="Default")
	bool bDrawNavBox = false;
	UPROPERTY(EditDefaultsOnly, Category="Default")
	FVector NavExtent = FVector(300.f, 300.f, 600.f);
	
	UPROPERTY()
	TObjectPtr<class UNavigationSystemV1> NavSystem;
};
