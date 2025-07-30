// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraHUD.generated.h"

class UAttributeMenuWidgetController;
class UAuraUserWidget;
class UOverlayWidgetController;

struct FWidgetControllerParams;

/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()
public:
	/** Bind callbacks if not yet and return controller */
	UOverlayWidgetController* CreateOrGetOverlayWC(const FWidgetControllerParams& WCParams);
	UAttributeMenuWidgetController* CreateOrGetAttributeMenuWC(const FWidgetControllerParams& WCParams);

	// Create and Set Overlay's WidgetController, then broadcast initial values
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAuraAbilitySystemComponent* ASC, UAuraAttributeSet* AS);
protected:

private:
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;
	UPROPERTY(EditAnywhere, Category = "AuraWidget|Overlay")
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	UPROPERTY(EditAnywhere, Category = "AuraWidget|Overlay")
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;


	// Widget created in OverlayWidget BP
	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;
	UPROPERTY(EditAnywhere, Category = "AuraWidget|AttributeMenu")
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
public:
	UFUNCTION(Category = "AuraWidget|Overlay")
	UAuraUserWidget* GetOverlayWidget() {return OverlayWidget;}
};
