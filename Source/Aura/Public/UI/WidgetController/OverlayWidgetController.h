// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraAttributeDelegate.h"
#include "UI/Data/MessageInfo.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

class UMessageInfo;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetInfoSignature, FAuraMessageInfo, Info);

/**
 *  Created in AuraHUD
 */
UCLASS()
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BindCallbacksDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FMessageWidgetInfoSignature MessageWidgetInfoDelegate;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GAS|WidgetData")
	TObjectPtr<UMessageInfo> MessageInfo;
};
