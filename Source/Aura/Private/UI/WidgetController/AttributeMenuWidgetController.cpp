// Copyright Hung


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeDataAsset.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

void UAttributeMenuWidgetController::BindCallbacksDependencies()
{
	GetPlayerState()->OnAttributePointsChangedDelegate.RemoveAll(this);
	GetPlayerState()->OnAttributePointsChangedDelegate.AddLambda([&](const int32 Points)
	{AttributePointsToUIDelegate.Broadcast(AttributePoints = Points, GetTotalPointsAllocating());});

	for (const auto& [Tag, AttributeData] : AuraHUD->GetAttributeDataList())
	{
		GetASC()->GetGameplayAttributeValueChangeDelegate(AttributeData.GameplayAttribute).RemoveAll(this);
		GetASC()->GetGameplayAttributeValueChangeDelegate(AttributeData.GameplayAttribute).AddLambda(
		[&](const FOnAttributeChangeData& Data)
		{
			if (Data.NewValue == Data.OldValue) return;
			AttributeInfoDelegate.Broadcast(Tag, Data.NewValue, AttributeData);
		});
	}
	GetASC()->OnApplyingStatFinishedDelegate.RemoveAll(this);
	GetASC()->OnApplyingStatFinishedDelegate.AddUObject(this, &UAttributeMenuWidgetController::BroadcastInitialValues);
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	bIsApplying = false;
	for (const auto& [Tag, AttributeData] : AuraHUD->GetAttributeDataList())
	{
		const float AttributeValue = AttributeData.GameplayAttribute.GetNumericValue(GetAttributeSet());
		AttributeInfoDelegate.Broadcast(Tag, AttributeValue, AttributeData);
	}

	AttributePoints = GetPlayerState()->GetAttributePoints();
	PointAllocationList.Reset();
	AttributePointsToUIDelegate.Broadcast(AttributePoints, 0);
}

int32& UAttributeMenuWidgetController::FindPointAllocationByTag(const FGameplayTag& Tag, bool& bFound)
{
	for (auto& [AttributeTag, AddedPoints] : PointAllocationList)
	{
		if (Tag.MatchesTagExact(AttributeTag))
		{
			bFound = true;
			return AddedPoints;
		}
	}
	bFound = false;
	return ZeroInteger;
}

int32 UAttributeMenuWidgetController::GetTotalPointsAllocating()
{
	int32 TotalPoints = 0;
	for (const auto& [AttributeTag, AddedPoints] : PointAllocationList)
	{
		TotalPoints += AddedPoints;
	}
	return TotalPoints;
}

void UAttributeMenuWidgetController::ApplyUpgrades()
{
	bIsApplying = false;
	GetASC()->ServerUpgradeAttribute(PointAllocationList);
	PointAllocationList.Reset();
}

void UAttributeMenuWidgetController::AllocatePointToAttribute(const FGameplayTag& AttributeTag, const int32 Points)
{
	if (Points == 0) return;
	bool bFound = false;
	int32& AttributeAllocation = FindPointAllocationByTag(AttributeTag, bFound);
	if (bFound)
	{
		const int32 PointsAboutToUse = GetTotalPointsAllocating() + Points;
		if (PointsAboutToUse < 0 || PointsAboutToUse > AttributePoints) return; // Invalid Points
		AttributeAllocation += Points;
		if (AttributeAllocation == 0) // Remove from array if 0
		{
			PointAllocationList.RemoveSingleSwap(FPointAllocation(AttributeTag, 0));
		}
		AttributePointsToUIDelegate.Broadcast(AttributePoints, PointsAboutToUse);
	}
	else
	{
		if (Points < 0 || Points > AttributePoints) return;
		PointAllocationList.AddUnique(FPointAllocation(AttributeTag, Points));
		AttributePointsToUIDelegate.Broadcast(AttributePoints, GetTotalPointsAllocating());
	}
}
