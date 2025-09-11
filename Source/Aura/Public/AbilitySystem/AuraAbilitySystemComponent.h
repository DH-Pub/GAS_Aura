// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

struct FPlayerAbilityData;
class UCostCooldownAbility;
class UInputAction;
enum class ETriggerEvent : uint8;
class UAuraGameplayAbility;
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityDataSignature, const FAuraAbilityData&, Data, const FPlayerAbilityData&, PlayerData);
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
private:
	// Remote Procedure Calls (RPCs) Broadcast EffectAssetTags
	UFUNCTION(Client, Unreliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec,
		FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

public:
	void AddCharacterAbilities(const TArray<TSubclassOf<UCostCooldownAbility>>& StartupAbilities); // Add Startup Abilities in PossessedBy
	void AddCharacterPassives(const TArray<TSubclassOf<UAuraGameplayAbility>>& StartupPassives); // Add Startup Passives
	void UnlockAbilityByLevel(int32 CharacterLevel);

	void AbilityInputTagTrigger(const ETriggerEvent TriggerEvent, const FGameplayTag& InputTag, UInputAction* InputAction);

	// Get ActivatableAbility Spec from Tag
	FGameplayAbilitySpec* GetSpecFromAssetTag(const FGameplayTag& AbilityTag);
	// return FGameplayTag::EmptyTag if not assigned to any input
	const FGameplayTag& GetInputFromSpec(const FGameplayAbilitySpec* Spec);


#pragma region Abilities Function
	UFUNCTION(BlueprintCallable)
	void ReduceCooldownByTag(const FGameplayTagContainer& TagContainer, const float Amount = 0.f, const float Percent = 0.f);
#pragma endregion
protected:
	// virtual void OnRep_ActivateAbilities() override;
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;


public:
	UPROPERTY(BlueprintAssignable, Category = "GAS|AbilityData")
	FAbilityDataSignature AbilityDataDelegate;// Send AbilityData (Icon, Tag, ...)
	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityData(const FGameplayAbilitySpec& AbilitySpec) const;
	
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const TArray<FPointAllocation>& PointsAllocated, AAuraPlayerState* AuraPS); // apply upgrade from server
	UFUNCTION(Client, Reliable)
	void ClientFinishUpgradeAttribute(const AAuraPlayerState* AuraPS); // Called in Server RPC to broadcast back to client

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoints(const FGameplayTag& AbilityTag, AAuraPlayerState* AuraPS);
	UFUNCTION(Server, Reliable)
	void ServerChangeAbilitySlot(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag = FGameplayTag::EmptyTag);
};
