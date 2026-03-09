// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilitySystemGlobals.generated.h"

/**
 * Project Settings: Game - Gameplay Abilities Settings:
 * -> Gameplay: Ability System Global Class
 * -> Gameplay Cue: Gameplay Cue Notify Paths: Copy Path from the folder containing all GC_...
 *		(or DefaultGame.ini +GameplayCueNotifyPaths=/Game/Blueprints/AbilitySystem/GameplayCues)
 */
UCLASS()
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
public:
	/** Searches the passed in actor for an ability system component */
	static class UAuraAbilitySystemComponent* GetAuraASC(const AActor* Actor);

	virtual FGameplayAbilityActorInfo* AllocAbilityActorInfo() const override;
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override; //REQUIRED: for FAuraEffectContext

	virtual UGameplayCueManager* GetGameplayCueManager() override; //REQUIRED: for UAuraCueManager

	virtual void InitGameplayCueParameters(FGameplayCueParameters& CueParameters, const FGameplayEffectContextHandle& EffectContext) override;
};
