// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

/**
 * Used to broadcast Effects (Health potions, ...)
 * @param AssetTags: Tags of effect
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */);

DECLARE_MULTICAST_DELEGATE_TwoParams(FAbilityDataSignature, const FGameplayAbilitySpec& /*AbilitySpec*/,
	const struct FAuraAbilityData& /*Data*/);
/**
 * For UI to Receive from FAbilityDataSignature AbilityDataDelegate
 * For Widget to receive info regardless of which ASC, WidgetController will handle binding to ASC
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReceiveAbilityDataSignature, const FGameplayAbilitySpec&, AbilitySpec,
	const FAuraAbilityData&, Data);
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
	virtual void OnGameplayEffectDurationChange(FActiveGameplayEffect& ActiveEffect) override;
	virtual void OnPredictiveGameplayCueCatchup(FGameplayTag Tag) override;

	virtual void ClientActivateAbilityFailed_Implementation(FGameplayAbilitySpecHandle AbilityToActivate, int16 PredictionKey) override;
protected:
	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;



public:
	UFUNCTION(Server, Reliable)
	void ServerCheckOwnedTags(FGameplayTagContainer TagsToCheck);
	UFUNCTION(Client, Reliable)
	void ClientRemoveTags(FGameplayTagContainer TagsToRemove);

	FAbilityDataSignature AbilityDataDelegate;// Send AbilityData to UI (Icon, Tag, ...)
	void BindAbilityDataDelegateToUIDelegate(UObject* InUserObject, FOnReceiveAbilityDataSignature& InDelegate);

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
