// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "AuraCueManager.generated.h"

USTRUCT()
struct FHandleGameplayCue
{
	GENERATED_BODY()

	FHandleGameplayCue(){}
	FHandleGameplayCue(const FGameplayTag& InTag, const EGameplayCueEvent::Type InEventType,
		const FGameplayCueParameters& InParams, const EGameplayCueExecutionOptions InOptions) :
		GameplayCueTag(InTag), EventType(InEventType), Parameters(InParams), Options(InOptions) {}

	TWeakObjectPtr<APlayerState> PlayerState;
	TWeakObjectPtr<AActor> Target;
	FGameplayTag GameplayCueTag;
	EGameplayCueEvent::Type EventType = EGameplayCueEvent::Executed;
	FGameplayCueParameters Parameters;
	EGameplayCueExecutionOptions Options = EGameplayCueExecutionOptions::Default;
};

/**
 * Set in ProjectSettings -> Game - GameplayAbilitiesSettings -> GameplayCue -> Global GameplayCue Manager Class
 * override GetGameplayCueManager from UAuraAbilitySystemGlobals
 */
UCLASS()
class AURA_API UAuraCueManager : public UGameplayCueManager
{
	GENERATED_BODY()
public:
	virtual bool ShouldSyncLoadMissingGameplayCues() const override {return false;}
	virtual bool ShouldAsyncLoadMissingGameplayCues() const override {return true;}

	// Called in ApplyGameplayEffectSpecToSelf()
	virtual void InvokeGameplayCueExecuted_FromSpec(UAbilitySystemComponent* OwningComponent,
		const FGameplayEffectSpec& Spec, FPredictionKey PredictionKey) override;

	virtual void HandleGameplayCue(AActor* TargetActor, FGameplayTag GameplayCueTag, EGameplayCueEvent::Type EventType,
		const FGameplayCueParameters& Parameters, EGameplayCueExecutionOptions Options = EGameplayCueExecutionOptions::Default) override;
	/*virtual AGameplayCueNotify_Actor* GetInstancedCueActor(AActor* TargetActor, UClass* GameplayCueNotifyActorClass,
		const FGameplayCueParameters& Parameters) override;*/

protected:
	UFUNCTION()
	void CueSetHandleGameplayCue(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

	TArray<FHandleGameplayCue> HandleCues;
};
