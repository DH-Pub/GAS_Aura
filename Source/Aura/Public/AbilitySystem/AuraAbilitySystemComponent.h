// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

USTRUCT()
struct FAuraCheckTags
{
	GENERATED_BODY()

	FAuraCheckTags(){}
	FAuraCheckTags(const FGameplayTag& InTag, const bool InExist) : Tag(InTag), bExist(InExist) {}
	UPROPERTY()
	FGameplayTag Tag;
	UPROPERTY()
	bool bExist = false;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		bOutSuccess |= Tag.NetSerialize_Packed(Ar, Map, bOutSuccess);
		Ar << bExist;
		return bOutSuccess;
	}
};
template<>
struct TStructOpsTypeTraits<FAuraCheckTags> : public TStructOpsTypeTraitsBase2<FAuraCheckTags>
{	// REQUIRED
	enum {WithNetSerializer = true};
};


/**
 * Used to broadcast Effects (Health potions, ...)
 * @param AssetTags: Tags of effect
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityDataSignature, const FGameplayAbilitySpec&, AbilitySpec,
	const FAuraAbilityData&, Data); //TODO: No need for this to be dynamic
DECLARE_MULTICAST_DELEGATE(FOnApplyingStatFinished)

DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag&);
/**
 *
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	void AddCharacterAbilities(const TArray<TSubclassOf<class UAuraGameplayAbility>>& StartupActives); // in PossessedBy

	TArray<FGameplayAbilitySpecHandle> InputHeldHandles; // Handles for Abilities that activates on holding input
	void AbilityInputPressed(const int32 InputID);
	void ProcessAbilityInput(const float DeltaTime, bool bGamePaused); // run every tick in APlayerController::PostProcessInput
	void AbilityInputReleased(const int32 InputID);
	void ClearInput();

	UFUNCTION(BlueprintCallable)
	void ReduceCooldownByTag(const FGameplayTagContainer& TagContainer, float Amount = 0.f, float Percent = 0.f);

	virtual FActiveGameplayEffectHandle ApplyGameplayEffectSpecToSelf(const FGameplayEffectSpec& GameplayEffect,
		FPredictionKey PredictionKey = FPredictionKey()) override;
	virtual void OnGameplayEffectDurationChange(struct FActiveGameplayEffect& ActiveEffect) override;
protected:
	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;
	// virtual void OnRep_ActivateAbilities() override;
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;

	virtual void OnPredictiveGameplayCueCatchup(FGameplayTag Tag) override;


public:
	FDeactivatePassiveAbility OnDeactivatePassive;

	FEffectAssetTags EffectAssetTags; // Convert OnGameplayEffectAppliedDelegateToSelf to Client RPC for UI use
	// Remote Procedure Calls (RPCs) Broadcast EffectAssetTags
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec,
		FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	UPROPERTY(BlueprintAssignable, Category = "GAS|AbilityData")
	FAbilityDataSignature AbilityDataDelegate;// Send AbilityData to UI (Icon, Tag, ...)
	void UpdateAbilityData(const FGameplayAbilitySpec& AbilitySpec) const;
	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityData(const FGameplayAbilitySpecHandle SpecHandle);

	void BroadcastAllAbilityData();
	FTimerHandle BroadcastDelegateTimer; // Prevent Repeated Call

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerCheckTags(FGameplayTagContainer TagsToCheck);
	UFUNCTION(Client, Reliable)
	void ClientResponseTagsCheck(const TArray<FAuraCheckTags>& TagsCount);
	UFUNCTION(Client, Reliable)
	void ClientUpdateOwnedTags(FGameplayTagContainer Tags);

	UFUNCTION(Server, Reliable)
	void ServerHandleGameplayEvent(const FGameplayTag& Tag, FGameplayAbilityTargetDataHandle DataHandle);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoints(const UGameplayAbility* AbilityCDO); // Send CDO because cannot send TSubclassOf
	UFUNCTION(Server, Reliable)
	void ServerChangeAbilitySlot(const UGameplayAbility* AbilityCDO, const int32 AbilityID);
	UFUNCTION(Client, Reliable)
	void ClientRefreshAbilityDataAndClearInput(); // Tell client to refresh after server finish update
	UFUNCTION(Client, Reliable)
	void ClientRefreshAbilityData(); // Tell client to refresh after server finish update
	FTimerHandle ClientRefreshTimer;
	bool bClearClientInput = false;
};
