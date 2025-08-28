// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class UInputAction;
enum class ETriggerEvent : uint8;
class UAuraInputAbility;
class UAuraGameplayAbility;
struct FUpgradeAllocation;
class AAuraPlayerState;
class UAuraAbilitySystemComponent;

/**
 * Used by AttributeMenuWidgetController
 * Placed inside AbilitySystemComponent to avoid circular dependencies
 * Because AuraWidgetController does #include "AbilitySystem/AuraAbilitySystemComponent.h"  
 */
USTRUCT(BlueprintType)
struct FPointAllocation
{
	GENERATED_BODY()
	
	FPointAllocation() {}
	FPointAllocation(const FGameplayTag& Tag, const int32 Points) : AttributeTag(Tag), AddedPoints(Points) {}
	
	UPROPERTY(BlueprintReadOnly, meta=(GameplayTagFilter="Attributes"))
	FGameplayTag AttributeTag;
	UPROPERTY(BlueprintReadOnly)
	int32 AddedPoints = 0;

	// This is so that TArray functions works RemoveSingleSwap, Find, ...
	bool operator==(const FPointAllocation& Other) const
	{
		return (AttributeTag == Other.AttributeTag
			&& AddedPoints == Other.AddedPoints);
	}
};

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGiveAbilitySignature, const FGameplayAbilitySpec&);
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AbilityActorInfoSet();
	
	FEffectAssetTags EffectAssetTags; // Convert OnGameplayEffectAppliedDelegateToSelf to Client RPC for UI use
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UAuraInputAbility>>& StartupAbilities); // Add Startup Abilities in PossessedBy
	void AddCharacterPassives(const TArray<TSubclassOf<UAuraGameplayAbility>>& StartupPassives); // Add Startup Passives
	
	FOnGiveAbilitySignature OnGiveAbilityDelegate; // used to broadcast AbilityDataDelegate
	
	void AbilityInputTagTrigger(const ETriggerEvent TriggerEvent, const FGameplayTag& InputTag, UInputAction* InputAction);

	UFUNCTION(BlueprintCallable)
	void ReduceCooldownByTag(const FGameplayTagContainer& TagContainer, const float Amount = 0.f, const float Percent = 0.f);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const TArray<FPointAllocation>& PointsAllocated, AAuraPlayerState* AuraPS); // apply upgrade from server
	UFUNCTION(Client, Reliable)
	void ClientFinishUpgrade(const AAuraPlayerState* AuraPS); // Called in Server RPC to broadcast back to client

	FGameplayAbilitySpec* GetSpecFromAssetTag(const FGameplayTag& AbilityTag); // Get ActivatableAbility Spec from Tag
	void UnlockAbilityByLevel(int32 CharacterLevel);

	const FGameplayTag& GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	const FGameplayTag& GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& StatusTag);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoints(const FGameplayTag& AbilityTag);
protected:
	virtual void OnRep_ActivateAbilities() override;
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	
    UFUNCTION(Client, Unreliable) // Remote Procedure Calls (RPCs)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec,
		FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
};
