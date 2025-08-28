// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraInputAbility.h"
#include "CostCooldownAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UCostCooldownAbility : public UAuraInputAbility
{
	GENERATED_BODY()
public:
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
	UPROPERTY(EditDefaultsOnly, Category="Costs|Aura")
	FScalableFloat ManaCost; // FGameplayEffectModifiedAttribute
	UPROPERTY(EditDefaultsOnly, Category="Costs|Aura")
	FScalableFloat HealthCost; // FGameplayEffectModifiedAttribute
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cooldowns|Aura")
	FScalableFloat CooldownDuration;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cooldowns|Aura", meta=(GameplayTagFilter="Ability"))
	FGameplayTagContainer CooldownTags; // If not set, GetCooldownTags() and ApplyCooldown() will use Super::
	
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags; // Temp container that we will return the (CooldownTags + Cooldown GE's CD Tags) pointer to in GetCooldownTags().

protected:
	UFUNCTION(BlueprintCallable)
	void GetCost(float& Mana, float& Health, const int32 InLevel = 1);
	UFUNCTION(BlueprintCallable, BlueprintPure=false) // const function automatically become BlueprintPure
	void GetCostOnLevelChanged(float& Mana, float& ManaChanged, float& Health, float& HealthChanged,
		const int32 InLevel = 1, const int32 LevelDelta = 0) const;
	UFUNCTION(BlueprintPure)
	float GetCooldown(const int32 InLevel = 1);
};
