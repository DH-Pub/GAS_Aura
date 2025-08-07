// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

class UAttributeDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAttributeInfoSignature, const FGameplayTag&, Tag, const FAuraAttributeData&, Info);
/**
 * Constructed in AuraHUD like OverlayWidgetController
 */
UCLASS()
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BindCallbacksDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

	TArray<FPointAllocation> PointAllocationList;
	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="bFound"))
	int32& FindPointAllocationByTag(const FGameplayTag& Tag, bool& bFound);
	int32 GetTotalPointsAllocating();
	UFUNCTION(BlueprintCallable)
	void ApplyUpgrades();

	UPROPERTY(BlueprintReadWrite)
	int32 AttributePoints = 0; // AttributePoints stored in AuraPlayerSate that will only change once apply
	UPROPERTY(BlueprintReadOnly)
	bool bIsApplying = false;
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnPlayerPointsChangedSignature AttributePointsToUIDelegate;
	UFUNCTION(BlueprintCallable)
	void AllocatePointToAttribute(UPARAM(meta=(GameplayTagFilter="Attributes")) const FGameplayTag& AttributeTag, int32 Points);
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnPlayerPointsChangedSignature SpellPointsToUIDelegate;
protected:
	UPROPERTY(EditDefaultsOnly, Category="GAS|WidgetData")
	TObjectPtr<UAttributeDataAsset> AttributeInfo;
private:
	int32 ZeroInteger; // Default Value if GetPointAllocationByTag() not found, DO NOT modify this if used
};
