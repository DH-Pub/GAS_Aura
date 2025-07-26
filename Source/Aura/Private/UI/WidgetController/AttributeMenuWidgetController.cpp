// Copyright Hung


#include "UI/WidgetController/AttributeMenuWidgetController.h"

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
		AttributePointsToUIDelegate.Broadcast(Points);
	});
	PS->OnSpellPointsChangedDelegate.AddLambda([this](const int32 Points)
	{
		SpellPointsToUIDelegate.Broadcast(Points);
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
	AttributePointsToUIDelegate.Broadcast(PS->GetAttributePoints());
	SpellPointsToUIDelegate.Broadcast(PS->GetSpellPoints());
}
