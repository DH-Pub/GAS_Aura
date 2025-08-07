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
	/**
	 * Set WidgetController and call Event WidgetControllerSet()
	 * BlueprintCallable so that multiple widget can use the same node (WBP_Health_Mana_Spells)
	 */
	UFUNCTION(BlueprintSetter, BlueprintCallable)
	virtual void SetWidgetController(UAuraWidgetController* InWidgetController);

	/**
	 * Widgets will have a Controller to receive delegate from, which might not be used and opted for Event Parameter instead
	 * Has BlueprintGetter and BlueprintSetter
	 */
	UPROPERTY(Setter=SetWidgetController, BlueprintSetter=SetWidgetController)
	TObjectPtr<UAuraWidgetController> WidgetController;
protected:
	/** Bind changed event to widget in BP */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void WidgetControllerSet(UAuraWidgetController* Controller);
};
