// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

class AAuraPlayerController;
class AAuraCharacterBase;
class IEnemyInterface;
class ICombatInterface;

/**
 * Gameplay Ability is only replicated to the owning player by default
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UAuraGameplayAbility();
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
	// This is Added to GetDynamicSpecSourceTags() during AddCharacterAbilities()
	UPROPERTY(EditDefaultsOnly, Category="Default", meta=(GameplayTagFilter="Input"))
	FGameplayTag StartupInputTag;

	
	// TODO: Make Separate GameplayAbility Class with Cooldown + Cost
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
	/*virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;*/

	// Projects may want to initiate passives or do other "BeginPlay" type of logic here.
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AAuraCharacterBase> AuraCharacterFromActorInfo = nullptr;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AAuraPlayerController> AuraPlayerController = nullptr;
};
