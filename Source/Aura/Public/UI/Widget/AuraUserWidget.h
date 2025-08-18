// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

class UAuraWidgetController;
/**
 * 
 */
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/* - Widgets will have a Controller to receive delegate from, which might not be used and opted for Event Parameter instead
	 * - BlueprintCallable (NOT BlueprintSetter) so that multiple widget can use the same node (WBP_Health_Mana_Spells)*/
	UFUNCTION(BlueprintCallable)
	virtual void SetWidgetController(UAuraWidgetController* InWidgetController);
protected:
	/** Bind changed event to widget in BP */
	UFUNCTION(BlueprintNativeEvent)
	void WidgetControllerSet(UAuraWidgetController* Controller);
};
