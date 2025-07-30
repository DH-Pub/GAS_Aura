// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

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
	UPROPERTY(EditDefaultsOnly, Category="Inputs", meta=(GameplayTagFilter="Inputs"))
	FGameplayTag StartupInputTag;

	
	// TODO: Make Separate GameplayAbility Class with Cooldown + Cost
	UPROPERTY(EditDefaultsOnly, Category="Costs|Aura")
	FScalableFloat Cost; // FGameplayEffectModifiedAttribute
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cooldowns|Aura")
	FScalableFloat CooldownDuration;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cooldowns|Aura", meta=(GameplayTagFilter="Cooldown"))
	FGameplayTagContainer CooldownTags; // If not set, GetCooldownTags() and ApplyCooldown() will use Super::
	
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags; // Temp container that we will return the (CooldownTags + Cooldown GE's CD Tags) pointer to in GetCooldownTags().
protected:
	/*virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;*/
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr) override;

	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<ICombatInterface> AvatarCombatInterface;
	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<IEnemyInterface> AvatarEnemyInterface;
	UPROPERTY(BlueprintReadOnly)
	AActor* AvatarActor = nullptr;
};
