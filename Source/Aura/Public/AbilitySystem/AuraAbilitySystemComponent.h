// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class AAuraCharacterBase;
struct FPlayerAbilityData;
class UInputAction;
enum class ETriggerEvent : uint8;
class UAuraGameplayAbility;

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

	// This is so that TArray functions works: RemoveSingleSwap, Find, ...
	bool operator==(const FPointAllocation& Other) const
	{
		return AttributeTag == Other.AttributeTag && AddedPoints == Other.AddedPoints;
	}
};

/**
 * Used to broadcast Effects (Health potions, ...)
 * @param AssetTags: Tags of effect
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityDataSignature, const FAuraAbilityData&, Data, const FPlayerAbilityData&, PlayerData);
DECLARE_MULTICAST_DELEGATE(FOnApplyingStatFinished)
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void InitAuraASC(AActor* InOwnerActor, AAuraCharacterBase* AuraCharacter);
	FEffectAssetTags EffectAssetTags; // Convert OnGameplayEffectAppliedDelegateToSelf to Client RPC for UI use
private:
	// Remote Procedure Calls (RPCs) Broadcast EffectAssetTags
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec,
		FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

public:
	void AddCharacterAbilities(const TArray<TSubclassOf<UAuraGameplayAbility>>& StartupActives); // Add Startup Abilities in PossessedBy
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
	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;
	// virtual void OnRep_ActivateAbilities() override;
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;


public:
	UPROPERTY(BlueprintAssignable, Category = "GAS|AbilityData")
	FAbilityDataSignature AbilityDataDelegate;// Send AbilityData (Icon, Tag, ...)
	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityData(const FGameplayAbilitySpec& AbilitySpec) const;

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const TArray<FPointAllocation>& PointsAllocated); // apply upgrade from server
	UFUNCTION(Client, Reliable)
	void ClientFinishUpgradeAttribute(); // Called in Server RPC to broadcast back to client
	FOnApplyingStatFinished OnApplyingStatFinishedDelegate; // for Attribute/Spell Points Finished Applying

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoints(const FGameplayTag& AbilityTag);
	UFUNCTION(Server, Reliable)
	void ServerChangeAbilitySlot(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag = FGameplayTag::EmptyTag);
};
