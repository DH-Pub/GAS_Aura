// Copyright Hung


#include "UI/WidgetController/EnemyWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"

void UEnemyWidgetController::BindCallbacksDependencies(UAuraAbilitySystemComponent* InASC)
{
	if (const UAuraAttributeSet* OldAS = GetAttributeSet())
	{
		AuraASC->GetGameplayAttributeValueChangeDelegate(OldAS->GetHealthAttribute()).RemoveAll(this);
		AuraASC->GetGameplayAttributeValueChangeDelegate(OldAS->GetMaxHealthAttribute()).RemoveAll(this);
	}

	Super::BindCallbacksDependencies(InASC);

	if (!AuraASC) return;
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
