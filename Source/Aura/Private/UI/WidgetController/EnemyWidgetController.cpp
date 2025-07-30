// Copyright Hung


#include "UI/WidgetController/EnemyWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"

void UEnemyWidgetController::BindCallbacksDependencies()
{
	BindGameplayAttributeToBroadcast(AttributeSet->GetHealthAttribute(), OnHealthChanged);
	BindGameplayAttributeToBroadcast(AttributeSet->GetMaxHealthAttribute(), OnMaxHealthChanged);
}

void UEnemyWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(AttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AttributeSet->GetMaxHealth());
}
