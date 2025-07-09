// Copyright Hung


#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	return UAuraWidgetController::CreateOrGetWidgetController<UOverlayWidgetController>(
		this,OverlayWidgetController, OverlayWidgetControllerClass, WCParams);
}
UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	return UAuraWidgetController::CreateOrGetWidgetController<UAttributeMenuWidgetController>(
		this, AttributeMenuWidgetController, AttributeMenuWidgetControllerClass, WCParams);
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out in BP_AuraHUD"));

	// Create and add HUD widget to viewport
	OverlayWidget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget->AddToViewport();

	 /**
	  * Create/Get WidgetController, BindCallbacksDependencies()
	  * Set WidgetController, Call WidgetControllerSet Event (Which will assign AbilityDataDelegate)
	  */
	OverlayWidget->SetWidgetController(GetOverlayWidgetController(FWidgetControllerParams(PC, PS, ASC, AS)));
	OverlayWidgetController->BroadcastInitialValues();
}
