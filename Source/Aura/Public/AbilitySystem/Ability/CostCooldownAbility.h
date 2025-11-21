// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "CostCooldownAbility.generated.h"

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
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

	UPROPERTY(EditDefaultsOnly, Category="Costs|Aura")
	FScalableFloat ManaCost; // FGameplayEffectModifiedAttribute
	UPROPERTY(EditDefaultsOnly, Category="Costs|Aura")
	FScalableFloat HealthCost; // FGameplayEffectModifiedAttribute

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cooldowns|Aura")
	FScalableFloat CooldownDuration;
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags; // Used to return FGameplayTagContainer* in GetCooldownTags()

	void GetCost(FAbilityDetails& Details) const;
	void GetCooldownAndReduction(FAbilityDetails& Details) const;
	void GetAbilityDetailsCostCooldown(FAbilityDetails& Details) const;
};
