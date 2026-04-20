// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Aura/Aura.h"
#include "AuraGameplayAbility.generated.h"

USTRUCT(BlueprintType)
struct FAbilityDetails
{
	GENERATED_BODY()
	FAbilityDetails(){}
	explicit FAbilityDetails(const int32 Level, const TWeakObjectPtr<UAbilitySystemComponent> InASC) :
		Level(Level), AbilitySystemComponent(InASC){}
	UPROPERTY(BlueprintReadOnly)
	int32 Level = 0;
	UPROPERTY(BlueprintReadOnly)
	float CostMana = 0.f;
	UPROPERTY(BlueprintReadOnly)
	float CostHealth = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Cooldown = 0.f;

	UPROPERTY(BlueprintReadOnly)
	int32 ProjectileNums = 0;
	UPROPERTY(BlueprintReadOnly)
	float Damage = 0.f;

	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent; // ASC for to check for cost/CD
};

UENUM(BlueprintType)
enum class EAuraActivationPolicy : uint8
{
	InputStart,
	InputHolding, // Continually try to activate ability while Input is Active (Triggered)
	OnSpawn, // Passive, activate in OnAvatarSet
};

// AOE, SingleTarget, AutoSelfCast, SkillShots, Toggle, Passive
/**
 * Base GameplayAbility for this project
 * Gameplay Ability is only replicated to the owning player by default
 */
UCLASS(Abstract)
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UAuraGameplayAbility();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AAuraCharacterBase> AuraCharacter = nullptr;

	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	EAuraActivationPolicy ActivationPolicy;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TEnumAsByte<EAuraAbilityInputID::Type> StartupInputID = EAuraAbilityInputID::None; //TODO: Testing, remove this

protected:
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr) override;
	const struct FAuraAbilityActorInfo* GetAuraActorInfo() const; // Not being used
	UFUNCTION(BlueprintCallable)
	class UAuraAbilitySystemComponent* GetAuraASC() const;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnAvatarSet();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnRemoveAbility();
public:
	virtual void GetAbilityDetails(FAbilityDetails& Details) const {};
	UFUNCTION(BlueprintImplementableEvent, Category="Description") // Override this in each ability's BP
	void GetDescription(const FAbilityDetails& Details, FText& OutDescription) const;
	UFUNCTION(BlueprintImplementableEvent, Category="Description") // Override this in each ability's BP
	void GetLevelChangeDescription(const FAbilityDetails& Details, const FAbilityDetails& ChangeDetails,
		FText& OutDescription) const;
};
