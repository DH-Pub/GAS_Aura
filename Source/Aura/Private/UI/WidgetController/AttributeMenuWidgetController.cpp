// Copyright Hung


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Ability/AttributesEventAbility.h"
#include "AbilitySystem/Data/AttributeDataAsset.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

void UAttributeMenuWidgetController::BindCallbacksDependencies()
{	// Super::BindCallbacksDependencies();
	AttributeTargetData.Data.Empty();
	GetPlayerState()->OnAttributePointsChangedDelegate.RemoveAll(this);
	GetPlayerState()->OnAttributePointsChangedDelegate.AddWeakLambda(this, [&](const int32 Points)
	{
		BroadcastInitialValues();
	});

	UAuraAbilitySystemComponent* ASC = GetASC();
	for (const auto& [Tag, AttributeData] : AuraHUD->GetAttributeDataList())
	{
		FOnGameplayAttributeValueChange& OnAttributeChanged =
			ASC->GetGameplayAttributeValueChangeDelegate(AttributeData.GameplayAttribute);
		OnAttributeChanged.RemoveAll(this);
		OnAttributeChanged.AddWeakLambda(this, [&](const FOnAttributeChangeData& Data)
		{
			if (Data.NewValue != Data.OldValue) AttributeInfoDelegate.Broadcast(Tag, Data.NewValue, AttributeData);
		});
	}
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	bIsApplying = false;
	for (const auto& [Tag, AttributeData] : AuraHUD->GetAttributeDataList())
	{
		const float AttributeValue = AttributeData.GameplayAttribute.GetNumericValue(GetAttributeSet());
		AttributeInfoDelegate.Broadcast(Tag, AttributeValue, AttributeData);
	}
	AttributePointsToUIDelegate.Broadcast(AttributePoints = GetPlayerState()->GetAttributePoints(),
		AttributeTargetData.TotalPointsAllocating());
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
	GetASC()->ServerHandleGameplayEvent(AuraGameplayTags::Attributes, FGameplayAbilityTargetDataHandle(Data));
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
