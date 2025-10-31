// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "Input/AuraInputComponent.h"
#include "CostCooldownAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UCostCooldownAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
protected:
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintCallable)
	virtual void FinishAbilityAction(); // Call this early (before EndAbility) so that this no longer block others
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cooldowns|Aura", meta=(GameplayTagFilter="Ability"))
	FGameplayTag CooldownTag; // If not set, GetCooldownTags() and ApplyCooldown() will use Super::
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags; // Temp container that we will return the (CooldownTags + Cooldown GE's CD Tags) pointer

	void GetCost(FAbilityDetails& Details) const;
	void GetCooldownAndReduction(FAbilityDetails& Details) const;
	void GetAbilityDetailsCostCooldown(FAbilityDetails& Details) const;



	/*
	 * Input ==========================================================================================================
	 */
#pragma region Input ==============================================
public:
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	// This is Added to GetDynamicSpecSourceTags() during AddCharacterAbilities()
	UPROPERTY(EditDefaultsOnly, Category="Default", meta=(GameplayTagFilter="Input"))
	FGameplayTag StartupInputTag; //TODO: Remove this or replace with HUD->AbilityData -> Input
#pragma endregion
};
