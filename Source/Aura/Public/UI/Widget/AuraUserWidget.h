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
	 * - C++ child override this function instead of WidgetControllerSet so that BP don't have to Add Call to Parent Function 
	 */
	UFUNCTION(BlueprintCallable)
	virtual void SetWidgetController(UAuraWidgetController* InWidgetController);
protected:
	/** Bind changed event to widget in BP */
	UFUNCTION(BlueprintNativeEvent)
	void WidgetControllerSet(UAuraWidgetController* Controller);
};
