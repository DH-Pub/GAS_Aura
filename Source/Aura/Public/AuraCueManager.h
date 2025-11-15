// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "AuraCueManager.generated.h"

/**
 * Set in ProjectSettings -> Game - GameplayAbilitiesSettings -> GameplayCue -> Global GameplayCue Manager Class
 * override GetGameplayCueManager from UAuraAbilitySystemGlobals
 */
UCLASS()
class AURA_API UAuraCueManager : public UGameplayCueManager
{
	GENERATED_BODY()
public:
	virtual void FlushPendingCues() override;
	
	virtual bool ShouldSyncLoadMissingGameplayCues() const override {return false;}
	virtual bool ShouldAsyncLoadMissingGameplayCues() const override {return true;}
	
	// Called in ApplyGameplayEffectSpecToSelf()
	virtual void InvokeGameplayCueExecuted_FromSpec(UAbilitySystemComponent* OwningComponent,
		const FGameplayEffectSpec& Spec, FPredictionKey PredictionKey) override;
	virtual void InvokeGameplayCueExecuted(UAbilitySystemComponent* OwningComponent, const FGameplayTag GameplayCueTag,
		FPredictionKey PredictionKey, FGameplayEffectContextHandle EffectContext) override;
	virtual void InvokeGameplayCueExecuted_WithParams(UAbilitySystemComponent* OwningComponent,
		const FGameplayTag GameplayCueTag, FPredictionKey PredictionKey, FGameplayCueParameters GameplayCueParameters) override;
	
	virtual void InvokeGameplayCueAddedAndWhileActive_FromSpec(UAbilitySystemComponent* OwningComponent,
		const FGameplayEffectSpec& Spec, FPredictionKey PredictionKey) override;

private:
	void AddPendingCueNextTick(FGameplayCuePendingExecute& PendingCue);
	FTimerHandle PendingHandle;
};
