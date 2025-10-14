// Copyright Hung


#include "UI/WidgetController/EnemyWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"

void UEnemyWidgetController::BindCallbacksDependencies()
{
	BindGameplayAttributeToBroadcast(GetAttributeSet()->GetHealthAttribute(), OnHealthChanged);
	BindGameplayAttributeToBroadcast(GetAttributeSet()->GetMaxHealthAttribute(), OnMaxHealthChanged);
}

void UEnemyWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAttributeSet()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAttributeSet()->GetMaxHealth());
}
