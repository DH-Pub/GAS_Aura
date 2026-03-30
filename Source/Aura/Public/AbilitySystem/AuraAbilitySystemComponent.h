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
		Ar << bExist;
		return bOutSuccess |= Tag.NetSerialize_Packed(Ar, Map, bOutSuccess);
	}
};
template<>
struct TStructOpsTypeTraits<FAuraCheckTags> : public TStructOpsTypeTraitsBase2<FAuraCheckTags>
{	// REQUIRED
	enum {WithNetSerializer = true};
};

USTRUCT()
struct FUnlockedAbilityData
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<class UAuraGameplayAbility> AbilityClass;
	UPROPERTY(VisibleAnywhere)
	int32 Level = 0;
};

/**
 * Used to broadcast Effects (Health potions, ...)
 * @param AssetTags: Tags of effect
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityDataSignature, const FGameplayAbilitySpec&, AbilitySpec,
	const FAuraAbilityData&, Data);
DECLARE_MULTICAST_DELEGATE(FOnApplyingStatFinished)
/**
 *
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	// Called on Server in PossessedBy
	void AddCharacterAbilities(const TArray<TSubclassOf<class UAuraGameplayAbility>>& StartupActives);

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
	virtual void OnPredictiveGameplayCueCatchup(FGameplayTag Tag) override;
protected:
	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;



public:
	UPROPERTY(BlueprintAssignable, Category = "GAS|AbilityData")
	FAbilityDataSignature AbilityDataDelegate;// Send AbilityData to UI (Icon, Tag, ...)
	void UpdateAbilityData(const FGameplayAbilitySpec& AbilitySpec) const;
	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityData(const FGameplayAbilitySpecHandle SpecHandle);

	void BroadcastAllAbilityData();
	FTimerHandle BroadcastDelegateTimer; // Prevent Repeated Call

	UFUNCTION(Server, Reliable)
	void ServerHandleGameplayEvent(const FGameplayTag& Tag, FGameplayAbilityTargetDataHandle DataHandle);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoints(const UGameplayAbility* AbilityCDO); // Send CDO because cannot send TSubclassOf
	UFUNCTION(Server, Reliable)
	void ServerChangeAbilitySlot(const UGameplayAbility* AbilityCDO, const int32 AbilityID);
	void HandlePassive(FGameplayAbilitySpec& Spec);

	UFUNCTION(Client, Reliable)
	void ClientRefreshAbilityData(const bool bClearInput); // Tell client to refresh after server finish update
	FTimerHandle ClientRefreshTimer;
	bool bClearClientInput = false;
};
