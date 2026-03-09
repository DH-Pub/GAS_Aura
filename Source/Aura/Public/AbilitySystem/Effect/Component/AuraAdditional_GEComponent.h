// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "AuraAdditional_GEComponent.generated.h"

/**
 * Based on UAdditionalEffectsGameplayEffectComponent
 * - Add Effect (mainly for GameplayCue) that will be removed when Owner GE is removed
 */
UCLASS(CollapseCategories, DisplayName="Aura Additional Effects")
class AURA_API UAuraAdditional_GEComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()
public:
	/**
	 * - Called when a GE is Added to the ActiveGameplayEffectsContainer (have duration or predicting locally).
	 * - Note: Also occurs because of replication (e.g. Server rep a GE to the client -- including the 'duplicate' GE after a prediction).
	 * - Return if the effect should remain active, or false to inhibit.
	 * - Note: Inhibit does not remove the effect (it remains added but dormant, waiting to un-inhibit).
	 */
	virtual bool OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer,
		FActiveGameplayEffect& ActiveGE) const override;

	/**
	 * - Called when a GE is executed. Only on ROLE_Authority. GE only Execute when applying an instant effect.
	 * - Note: Periodic effects Execute every period (and are also added to ActiveGameplayEffectsContainer).
	 * One may think of this as periodically executing an instant effect (and thus can only happen on the server).
	 */
	virtual void OnGameplayEffectExecuted(FActiveGameplayEffectsContainer& ActiveGEContainer,
		FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const override;

	/**
	 * - Called when GE is initially applied or stacked. This call does not happen periodically, nor through replication.
	 * - Should favor this over OnActiveGameplayEffectAdded & OnGameplayEffectExecuted.
	 */
	virtual void OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer,
		FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const override;

protected:
	// When ActiveGE is removed
	void OnActiveGameplayEffectRemoved(const struct FGameplayEffectRemovalInfo& RemovalInfo,
		FActiveGameplayEffectsContainer* ActiveGEContainer) const;

public:
	/**
	 * Global Cue Effect: GE mainly for adding GameplayCue Actor. Remove stack on Owner GE Removed.
	 * - The GE's FGameplayEffectCue Tag must be unique. Other GE's can share the same Global Cue Effect (stackable).
	 * - Effect must be Non-Instant, NO_PERIOD, and Stackable
	 *  1. Infinite (maybe HasDuration) for Cue WhileActive/Removed on Add/Remove GE
	 *  2. NO_PERIOD: if GE last tick on duration expired, might Execute (add) another GC after Removed
	 *  3. Stackable: if multiple GE's with same Global GECue apply, Cue will be removed for Effects ending earlier
	 *
	 * No need for Stun, which is non-periodic
	 */
	UPROPERTY(EditDefaultsOnly, Category="GameplayCues")
	TArray<TSubclassOf<UGameplayEffect>> GlobalCueEffects;


#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
