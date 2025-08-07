// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "EnemyWidgetController.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UEnemyWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BindCallbacksDependencies() override;
	virtual void BroadcastInitialValues() override;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnVitalAttributeChanged OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnVitalAttributeChanged OnMaxHealthChanged;
};
