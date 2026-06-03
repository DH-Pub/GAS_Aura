// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "GameplayEffectExecutionCalculation.h"
#include "NativeGameplayTags.h"
#include "CostCooldownAbility.generated.h"

AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Cooldown_Duration);

/**
 *
 */
UCLASS()
class AURA_API UCostCooldownAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UCostCooldownAbility();
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

	UPROPERTY(EditDefaultsOnly, Category="Costs|Aura", meta=(ForceInlineRow, GameplayTagFilter="Attributes"))
	TMap<FGameplayTag, FScalableFloat> AbilityCosts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cooldowns|Aura")
	FScalableFloat CooldownDuration;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cooldowns|Aura", meta=(GameplayTagFilter=""))
	FGameplayTagContainer CooldownTags;
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags; // Used to return FGameplayTagContainer* in GetCooldownTags()

private:
	void GetCostExecutionOutput(UAbilitySystemComponent* ASC, FGameplayEffectSpec& Spec,
		FGameplayEffectCustomExecutionOutput& ExecutionOutput) const;

public:
	void GetCost(FAbilityDetails& Details) const;

	void GetCooldown(FAbilityDetails& Details) const;
	/** This can be called on CDO */
	virtual void GetAbilityDetails(FAbilityDetails& Details) const override;
};
