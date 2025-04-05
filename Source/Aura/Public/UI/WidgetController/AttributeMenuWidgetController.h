// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraAttributeDelegate.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

class UAttributeInfo;

/**
 * Constructed in AuraHUD like OverlayWidgetController
 */
UCLASS()
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BindCallbacksDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;
protected:
	UPROPERTY(EditDefaultsOnly, Category="GAS|WidgetData")
	TObjectPtr<UAttributeInfo> AttributeInfo;
};
