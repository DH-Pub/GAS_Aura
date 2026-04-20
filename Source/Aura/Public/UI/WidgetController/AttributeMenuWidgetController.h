// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AttributesEventAbility.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttriubtePointsChanged, int32, NewValue, int32, PointsAllocating);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAttributeInfoSignature, const FGameplayTag&, Tag, float, NewValue, const FAuraAttributeData&, Info);
/**
 * Constructed in AuraHUD like OverlayWidgetController
 */
UCLASS()
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BindCallbacksDependencies(UAuraAbilitySystemComponent* InASC) override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

	FGameplayAbilityTargetData_AttributeData AttributeTargetData;
	UFUNCTION(BlueprintCallable)
	int32 FindPointAllocationByTag(const FGameplayTag& Tag);
	UFUNCTION(BlueprintCallable)
	void ApplyUpgrades();

	UPROPERTY(BlueprintReadWrite)
	int32 AttributePoints = 0; // AttributePoints stored in AuraPlayerSate that will only change once apply
	UPROPERTY(BlueprintReadOnly)
	bool bIsApplying = false; // Gray out buttons until Server send update
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttriubtePointsChanged AttributePointsToUIDelegate;
	UFUNCTION(BlueprintCallable)
	void AllocatePointToAttribute(UPARAM(meta=(GameplayTagFilter="Attributes")) const FGameplayTag& AttributeTag, int32 Points);
};
