// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

class UMessageInfo;
class UOverlay;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageTableSignature, const FMessageRow&, Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetInfoSignature, const FAuraMessageInfo&, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXPChangedSignature, int32, Level, float, XPPercent);
/**
 *  Created in AuraHUD, which is only accessible by local client
 */
UCLASS()
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BindCallbacksDependencies() override;
	virtual void BroadcastInitialValues() override;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnVitalAttributeChanged OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnVitalAttributeChanged OnMaxHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnVitalAttributeChanged OnManaChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnVitalAttributeChanged OnMaxManaChanged;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UOverlay> Overlay_Screen;
	UPROPERTY(BlueprintAssignable)
	FMessageWidgetInfoSignature MessageWidgetInfoDelegate; // Item Pickup Message
	UPROPERTY(BlueprintAssignable)
	FMessageTableSignature MessageTableDelegate; // Item Pickup Message
	
	UPROPERTY(BlueprintAssignable)
	FOnXPChangedSignature OnXPPercentChangedDelegate; // Send XP% and Level to UI
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|WidgetData")
	TObjectPtr<UMessageInfo> MessageInfo;
	UPROPERTY(EditDefaultsOnly, Category="Default|MessageData")
	TObjectPtr<UDataTable> MessageDataTable;
	
	UFUNCTION()
	void BroadcastXPToUI(int32 XP = 0) const;
};
