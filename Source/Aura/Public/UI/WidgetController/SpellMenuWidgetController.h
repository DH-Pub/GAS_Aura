// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

class UOverlayWidgetController;
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
protected:
	UPROPERTY(BlueprintReadOnly, BlueprintGetter=GetOverlayWidgetController)
	TObjectPtr<UOverlayWidgetController> OverlayWC;
	UFUNCTION(BlueprintGetter)
	UOverlayWidgetController* GetOverlayWidgetController();
};
