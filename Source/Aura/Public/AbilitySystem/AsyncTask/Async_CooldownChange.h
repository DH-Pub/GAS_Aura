// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Async_CooldownChange.generated.h"

struct FActiveGameplayEffectHandle;
struct FGameplayEffectSpec;
class UAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCooldownChanged, float, TimeRemaining, float, Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCooldownEnd);

/**
 *
 */
// UCLASS(MinimalAPI, meta=(ExposedAsyncProxy=AsyncTask))
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
	static UAsync_CooldownChange* WaitForCooldownChange(UAbilitySystemComponent* InASC,
		const FGameplayTagContainer& InCooldownTags, const bool InUseServerCooldown = false);

	UFUNCTION(BlueprintCallable)
	void EndTask();

	UFUNCTION(BlueprintCallable)
	void InitWaitCooldown();
private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	FGameplayTagContainer CooldownTags;
	bool bUseServerCooldown = false; // whether to wait for server to send cooldown information, not "predicted"

	float CooldownDuration = 0.f; // Original (or first) duration used to keep percentage of
	float CooldownTime = 0.f;

	void OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied,
		FActiveGameplayEffectHandle ActiveEffectHandle);
	void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount); // for when Effect is removed

	void CheckCooldown(const FActiveGameplayEffectHandle ActiveHandle);
};
