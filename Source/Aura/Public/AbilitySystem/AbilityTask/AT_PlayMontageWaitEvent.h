// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_PlayMontageWaitEvent.generated.h"

/** Delegate type used, EventTag and Payload may be empty if it came from the montage callbacks
 * This has to be used for all Delegates in Task to be able to send Params
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMontageWaitForEventDelegate, FGameplayTag, EventTag, const FGameplayEventData&, EventData);

/**
 * This task combines PlayMontageAndWait and WaitForEvent into one.
 * use FGameplayTagContainer so you can wait for multiple types of activations such as from a melee combo
 */
UCLASS()
class AURA_API UAT_PlayMontageWaitEvent : public UAbilityTask
{
	GENERATED_BODY()
public:
	/**
	* The Blueprint node for this task, PlayMontageAndWaitForEvent, has some black magic from the plugin that automagically calls Activate()
	* inside K2Node_LatentAbilityCall as stated in the AbilityTask.h. Ability logic written in C++ probably needs to call Activate() itself manually.
	*/
	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;

	UPROPERTY(BlueprintAssignable)
	FMontageWaitForEventDelegate OnCompleted;
	UPROPERTY(BlueprintAssignable)
	FMontageWaitForEventDelegate OnBlendOut;
	UPROPERTY(BlueprintAssignable)
	FMontageWaitForEventDelegate OnInterrupted;
	UPROPERTY(BlueprintAssignable)
	FMontageWaitForEventDelegate OnCancelled; /*Task explicitly cancelled by another ability*/

	/** One of the triggering gameplay events happened */
	UPROPERTY(BlueprintAssignable)
	FMontageWaitForEventDelegate EventReceived;

	/**
	 * Play a montage and wait for finish.
	 * If a gameplay event happens that matches EventTags (or EventTags is empty), EventReceived will fire.
	 * On normal execution, OnBlendOut is called when the montage is blending out, and OnCompleted when completely done playing
	 * OnInterrupted is called if another montage overwrites this, and OnCancelled is called if the ability or task is cancelled
	 *
	 * @param OwningAbility Owning Ability
	 * @param MontageToPlay The montage to play on the character
	 * @param EventTags Receive SendGameplayEventToActor, fire EventReceived. If empty, all events will trigger callback
	 * @param Rate Change to play the montage faster or slower
	 * @param StartSection If not empty, named montage section to start from
	 * @param bStopWhenAbilityEnds If true, aborted if ability ends normally. Always stopped when ability is cancelled.
	 * @param AnimRootMotionTranslationScale Change to modify size of root motion or set to 0 to block it entirely
	 */
	UFUNCTION(BlueprintCallable, meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="TRUE"))
	static UAT_PlayMontageWaitEvent* PlayMontageWaitEvent(class UAuraGameplayAbility* OwningAbility,
		UAnimMontage* MontageToPlay,
		UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory.Montage")) FGameplayTagContainer EventTags,
		const float Rate = 1.f, const FName StartSection = NAME_None,
		const bool bStopWhenAbilityEnds = true, const float AnimRootMotionTranslationScale = 1.f);

private:
	UPROPERTY()
	UAnimMontage* MontageToPlay;

	UPROPERTY()
	FGameplayTagContainer EventTags; /** List of tags to match against gameplay events */
	UPROPERTY()
	float Rate; /** Playback rate */
	UPROPERTY()
	FName StartSection; /** Section to start montage from */

	UPROPERTY()
	float AnimRootMotionTranslationScale; /** Modifies how root motion movement to apply */
	UPROPERTY()
	bool bStopWhenAbilityEnds; /** Rather montage should be aborted if ability ends */

	/** Checks if the ability is playing a montage and stops that montage, returns true if a montage was stopped, false if not. */
	bool StopPlayingMontage();

	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	void OnAbilityCancelled();

	FOnMontageEnded MontageEndedDelegate;
	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FDelegateHandle CancelledHandle;

	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);
	FDelegateHandle EventHandle;
};
