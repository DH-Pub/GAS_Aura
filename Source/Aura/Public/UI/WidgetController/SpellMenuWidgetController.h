// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

class UOverlayWidgetController;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellPointsChanged, int32, NewValue);
/**
 * 
 */
UCLASS()
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BindCallbacksDependencies() override;
	virtual void BroadcastInitialValues() override;
	
	UPROPERTY(BlueprintReadOnly, BlueprintGetter=GetOverlayWidgetController)
	TObjectPtr<UOverlayWidgetController> OverlayWC;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnSpellPointsChanged SpellPointsToUIDelegate;
protected:
	UFUNCTION(BlueprintGetter)
	UOverlayWidgetController* GetOverlayWidgetController();
};
