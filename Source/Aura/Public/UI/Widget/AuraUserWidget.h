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
	// Set WidgetController and call Event WidgetControllerSet()
	UFUNCTION(BlueprintCallable)
	virtual void SetWidgetController(UAuraWidgetController* InWidgetController);

	/** Widget will have a Controller that will send delegate to it */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAuraWidgetController> WidgetController;
protected:
	/** Bind changed event to widget in BP */
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
