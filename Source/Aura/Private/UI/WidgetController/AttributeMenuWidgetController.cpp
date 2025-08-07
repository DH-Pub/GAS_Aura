// Copyright Hung


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeDataAsset.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksDependencies()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	for (TTuple<FGameplayTag, FAuraAttributeData>& Pair : AttributeInfo.Get()->AttributeDataList)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value.GameplayAttribute).AddLambda(
			[this, &Pair, AS](const FOnAttributeChangeData& Data)
			{
				Pair.Value.AttributeValue = Pair.Value.GameplayAttribute.GetNumericValue(AS);
				AttributeInfoDelegate.Broadcast(Pair.Key, Pair.Value);
			}
		);
	}
	AAuraPlayerState* PS = CastChecked<AAuraPlayerState>(PlayerState);
	PS->OnAttributePointsChangedDelegate.AddLambda([this](const int32 Points)
	{
		AttributePoints = Points;
		AttributePointsToUIDelegate.Broadcast(AttributePoints, GetTotalPointsAllocating());
	});
	PS->OnSpellPointsChangedDelegate.AddLambda([this](const int32 Points)
	{
		//TODO: Modify PointAllocating
		SpellPointsToUIDelegate.Broadcast(Points, 0);
	});
	
	PS->OnApplyingStatFinishedDelegate.AddLambda([this]()
	{
		bIsApplying = false;
		BroadcastInitialValues();
	});
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	check(AttributeInfo)

	for (TTuple<FGameplayTag, FAuraAttributeData>& Pair : AttributeInfo.Get()->AttributeDataList)
	{
		Pair.Value.AttributeValue = Pair.Value.GameplayAttribute.GetNumericValue(AS);
		AttributeInfoDelegate.Broadcast(Pair.Key, Pair.Value);
	}
	
	AAuraPlayerState* PS = CastChecked<AAuraPlayerState>(PlayerState);

	AttributePoints = PS->GetAttributePoints();
	PointAllocationList.Empty();
	AttributePointsToUIDelegate.Broadcast(AttributePoints, 0);
	
	SpellPointsToUIDelegate.Broadcast(PS->GetSpellPoints(), 0);
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
	bIsApplying = true;
	CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent)->UpgradeAttribute(PointAllocationList);
	PointAllocationList.Empty();
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
