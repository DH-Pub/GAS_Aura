// Copyright Hung


#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::CreateOrGetOverlayWC(const FWidgetControllerParams& WCParams)
{
	return UAuraWidgetController::CreateOrGetWidgetController<UOverlayWidgetController>(
		this,OverlayWidgetController, OverlayWidgetControllerClass, WCParams);
}
UAttributeMenuWidgetController* AAuraHUD::CreateOrGetAttributeMenuWC(const FWidgetControllerParams& WCParams)
{
	return UAuraWidgetController::CreateOrGetWidgetController<UAttributeMenuWidgetController>(
		this, AttributeMenuWidgetController, AttributeMenuWidgetControllerClass, WCParams);
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAuraAbilitySystemComponent* ASC, UAuraAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out in BP_AuraHUD"));

	// Create and add HUD widget to viewport
	OverlayWidget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget->AddToViewport();

	 /**
	  * Create/Get WidgetController, BindCallbacksDependencies()
	  * Set WidgetController, Call WidgetControllerSet Event (Which will assign AbilityDataDelegate)
	  */
	OverlayWidget->SetWidgetController(CreateOrGetOverlayWC(FWidgetControllerParams(PC, PS, ASC, AS)));
	OverlayWidgetController->BroadcastInitialValues();
}
