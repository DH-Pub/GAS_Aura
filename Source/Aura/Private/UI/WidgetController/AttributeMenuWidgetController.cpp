// Copyright Hung


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BindCallbacksDependencies()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	for (TTuple<FGameplayTag, FAuraAttributeInfo>& Pair : AttributeInfo.Get()->AttributeInformation)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value.GameplayAttribute).AddLambda(
			[this, &Pair, AS](const FOnAttributeChangeData& Data)
			{
				Pair.Value.AttributeValue = Pair.Value.GameplayAttribute.GetNumericValue(AS);
				AttributeInfoDelegate.Broadcast(Pair.Key, Pair.Value);
			}
		);
	}
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	check(AttributeInfo)

	for (TTuple<FGameplayTag, FAuraAttributeInfo>& Pair : AttributeInfo.Get()->AttributeInformation)
	{
		Pair.Value.AttributeValue = Pair.Value.GameplayAttribute.GetNumericValue(AS);
		AttributeInfoDelegate.Broadcast(Pair.Key, Pair.Value);
	}
}
