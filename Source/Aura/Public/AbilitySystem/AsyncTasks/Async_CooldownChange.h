// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Async_CooldownChange.generated.h"

struct FActiveGameplayEffect;
struct FActiveGameplayEffectHandle;
struct FGameplayEffectSpec;
class UAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCooldownChanged, float, TimeRemaining, float, Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCooldownEnd);

/**
 * 
 */
UCLASS(meta=(ExposedAsyncProxy=AsyncTask))
class AURA_API UAsync_CooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FCooldownChanged CooldownChanged;
	UPROPERTY(BlueprintAssignable)
	FCooldownEnd CooldownEnd;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
	static UAsync_CooldownChange* WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayTagContainer& InCooldownTags, bool InUseServerCooldown = false);

	UFUNCTION(BlueprintCallable)
	void EndTask();

	UFUNCTION(BlueprintCallable)
	void BroadcastInitialCooldown() const;
private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	FGameplayTagContainer CooldownTags;
	bool bUseServerCooldown = false; // whether to wait for server to send cooldown information, not "predicted"
	
	float CooldownDuration = 0.f;
	float CooldownTime = 0.f;

	void OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle);
	void OnGameplayEffectRemoved(const FActiveGameplayEffect& ActiveEffect);
	void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount);
};
