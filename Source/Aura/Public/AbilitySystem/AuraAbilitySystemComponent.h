// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityDataSignature, const FAuraAbilityData&, Data, const struct FPlayerAbilityData&, PlayerData);
DECLARE_MULTICAST_DELEGATE(FOnApplyingStatFinished)
/**
 *
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void InitAuraASC(AActor* InOwnerActor, class AAuraCharacterBase* AuraCharacter);
	FEffectAssetTags EffectAssetTags; // Convert OnGameplayEffectAppliedDelegateToSelf to Client RPC for UI use
private:
	// Remote Procedure Calls (RPCs) Broadcast EffectAssetTags
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec,
		FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

public:
	void AddCharacterAbilities(const TArray<TSubclassOf<class UAuraGameplayAbility>>& StartupActives); // in PossessedBy

protected:
	TArray<FGameplayAbilitySpecHandle> InputHeldHandles; // Handles for Abilities that activates on holding input
public:
	void AbilityInputPressed(const int8 InputID);
	void ProcessAbilityInput(const float DeltaTime, bool bGamePaused); // run per-frame APlayerController::PostProcessInput
	void AbilityInputReleased(const int8 InputID);

	// Get ActivatableAbility Spec from Tag
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);

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
	void ClientUpdateAbilityData(const FGameplayAbilitySpec& AbilitySpec);
	void UpdateAbilityData(const FGameplayAbilitySpec& AbilitySpec) const;

	void BroadcastAllAbilityData();
	FTimerHandle BroadcastDelegate;

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const TArray<FPointAllocation>& PointsAllocated); // apply upgrade from server
	UFUNCTION(Client, Reliable)
	void ClientFinishUpgradeAttribute(); // Called in Server RPC to broadcast back to client
	FOnApplyingStatFinished OnApplyingStatFinishedDelegate; // for Attribute/Spell Points Finished Applying

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoints(const FGameplayTag& AbilityTag);
	UFUNCTION(Server, Reliable)
	void ServerChangeAbilitySlot(const FGameplayTag& AbilityTag, const int32 AbilityID);
	UFUNCTION(Client, Reliable)
	void ClientRefreshAbilityData();

public:
	UFUNCTION(BlueprintCallable)
	bool TryActivateAbilityByDynamicTag(const FGameplayTag& Tag, const bool bAllowRemoteActivation = true);
	virtual void NetMulticast_InvokeGameplayCueExecuted_WithParams_Implementation(const FGameplayTag GameplayCueTag,
		FPredictionKey PredictionKey, FGameplayCueParameters GameplayCueParameters) override;
};
