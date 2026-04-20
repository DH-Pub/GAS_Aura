// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Async_CooldownChange.generated.h"

class UAuraAbilitySystemComponent;
struct FActiveGameplayEffectHandle;
struct FGameplayEffectSpec;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCooldownChanged, float, TimeRemaining, float, Duration);

/**
 *
 */
UCLASS(BlueprintType, /*MinimalAPI,*/ meta=(ExposedAsyncProxy=AsyncTask))
class AURA_API UAsync_CooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FCooldownChanged CooldownChanged;
	UPROPERTY(BlueprintAssignable)
	FCooldownChanged CooldownEnd;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
	static UAsync_CooldownChange* WaitForCooldownChange(UAuraAbilitySystemComponent* InASC,
		const FGameplayTagContainer& InCooldownTags, const bool InUseServerCooldown = false);

	UFUNCTION(BlueprintCallable)
	void EndTask();

	UFUNCTION(BlueprintCallable)
	void InitWaitCooldown();
private:
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> ASC;
	FGameplayTagContainer CooldownTags;
	bool bUseServerCooldown = false; // whether to wait for server to send cooldown information, not "predicted"

	float CooldownDuration = 0.f; // Original (or first) duration used to keep percentage of
	float CooldownTime = 0.f;

	void OnActiveEffectAdded(class UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied,
		FActiveGameplayEffectHandle ActiveEffectHandle);

	void CheckCooldown();
};
