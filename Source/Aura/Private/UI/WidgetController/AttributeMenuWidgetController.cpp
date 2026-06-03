// Copyright Hung


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraTag.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Ability/AttributesEventAbility.h"
#include "AbilitySystem/Data/AttributeDataAsset.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

void UAttributeMenuWidgetController::UnbindOldAbilitySystemComponent()
{
	AttributeTargetData.Data.Empty();
	if (AAuraPlayerState* OldPS = GetPlayerState())
	{
		OldPS->OnAttributePointsChangedDelegate.RemoveAll(this);
	}
	for (const auto& [Tag, AttributeData] : AAuraHUD::Get(this)->GetAttributeDataList())
	{
		AuraASC->GetGameplayAttributeValueChangeDelegate(AttributeData.GameplayAttribute).RemoveAll(this);
	}
}

void UAttributeMenuWidgetController::BindCallbacksDependencies()
{
	AttributeTargetData.Data.Empty();
	GetPlayerState()->OnAttributePointsChangedDelegate.AddWeakLambda(this, [&](const int32 Points)
	{
		bIsApplying = false;
		for (const auto& [Tag, AttributeData] : AAuraHUD::Get(this)->GetAttributeDataList())
		{
			const float AttributeValue = AttributeData.GameplayAttribute.GetNumericValue(GetAttributeSet());
			AttributeInfoDelegate.Broadcast(Tag, AttributeValue, AttributeData);
		}
		AttributePointsToUIDelegate.Broadcast(AttributePoints = Points, AttributeTargetData.TotalPointsAllocating());
	});

	for (const auto& [Tag, AttributeData] : AAuraHUD::Get(this)->GetAttributeDataList())
	{
		AuraASC->GetGameplayAttributeValueChangeDelegate(AttributeData.GameplayAttribute).AddWeakLambda(
		this, [&](const FOnAttributeChangeData& Data)
		{
			if (Data.NewValue != Data.OldValue) AttributeInfoDelegate.Broadcast(Tag, Data.NewValue, AttributeData);
		});
	}
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	if (GetPlayerState()) GetPlayerState()->BroadcastCurrentData();
}

int32 UAttributeMenuWidgetController::FindPointAllocationByTag(const FGameplayTag& Tag)
{
	const int32* PointPtr = AttributeTargetData.FindPointsPtr(Tag);
	return PointPtr ? *PointPtr : 0;
}

void UAttributeMenuWidgetController::ApplyUpgrades()
{
	bIsApplying = true;
	FGameplayAbilityTargetData_AttributeData* Data = new FGameplayAbilityTargetData_AttributeData();
	*Data = AttributeTargetData;
	AttributeTargetData.Data.Empty();
	// Broadcast to disable buttons
	AttributePointsToUIDelegate.Broadcast(AttributePoints - Data->TotalPointsAllocating(), 0);
	GetASC()->ServerHandleGameplayEvent(AuraTag::Attributes, FGameplayAbilityTargetDataHandle(Data));
}

void UAttributeMenuWidgetController::AllocatePointToAttribute(const FGameplayTag& AttributeTag, const int32 Points)
{
	if (Points == 0) return;
	if (int32* PointsPtr = AttributeTargetData.FindPointsPtr(AttributeTag))
	{
		const int32 PointsAboutToUse = AttributeTargetData.TotalPointsAllocating() + Points;
		if (PointsAboutToUse < 0 || PointsAboutToUse > AttributePoints) return; // Invalid Points
		*PointsPtr += Points;
		AttributeTargetData.Data.RemoveSwap(FAttributeData(AttributeTag, 0)); // Remove if above 0
	}
	else if (Points > 0 && Points <= AttributePoints)
	{
		AttributeTargetData.AddNewData(AttributeTag, Points);
	}
	AttributePointsToUIDelegate.Broadcast(AttributePoints, AttributeTargetData.TotalPointsAllocating());
}
