// Copyright Hung


#include "UI/WidgetController/EnemyWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"

void UEnemyWidgetController::BindCallbacksDependencies()
{
	if (const UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		BindGameplayAttributeToBroadcast(AS->GetHealthAttribute(), OnHealthChanged);
		BindGameplayAttributeToBroadcast(AS->GetMaxHealthAttribute(), OnMaxHealthChanged);
	}
}

void UEnemyWidgetController::BroadcastInitialValues()
{
	if (const UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		OnHealthChanged.Broadcast(AS->GetHealth());
		OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
	}
}
