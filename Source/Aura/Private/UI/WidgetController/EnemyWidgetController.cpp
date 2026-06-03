// Copyright Hung


#include "UI/WidgetController/EnemyWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"

void UEnemyWidgetController::UnbindOldAbilitySystemComponent()
{
	if (const UAuraAttributeSet* OldAS = GetAttributeSet())
	{
		AuraASC->GetGameplayAttributeValueChangeDelegate(OldAS->GetHealthAttribute()).RemoveAll(this);
		AuraASC->GetGameplayAttributeValueChangeDelegate(OldAS->GetMaxHealthAttribute()).RemoveAll(this);
	}
}

void UEnemyWidgetController::BindCallbacksDependencies()
{
	if (const UAuraAttributeSet* NewAS = GetAttributeSet())
	{
		BindGameplayAttributeToBroadcast(NewAS->GetHealthAttribute(), OnHealthChanged);
		BindGameplayAttributeToBroadcast(NewAS->GetMaxHealthAttribute(), OnMaxHealthChanged);
	}
}

void UEnemyWidgetController::BroadcastInitialValues()
{
	if (const UAuraAttributeSet* NewAS = GetAttributeSet())
	{
		OnHealthChanged.Broadcast(NewAS->GetHealth());
		OnMaxHealthChanged.Broadcast(NewAS->GetMaxHealth());
	}
}
