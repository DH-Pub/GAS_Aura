// Copyright Hung


#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

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
USpellMenuWidgetController* AAuraHUD::CreateOrGetSpellMenuWC(const FWidgetControllerParams& WCParams)
{
	return UAuraWidgetController::CreateOrGetWidgetController<USpellMenuWidgetController>(
		this, SpellMenuWidgetController, SpellMenuWidgetControllerClass, WCParams);
}

void AAuraHUD::InitOverlay(const FWidgetControllerParams& WCParams)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out in BP_AuraHUD"));

	// Create and add HUD widget to viewport
	OverlayWidget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget->AddToViewport();

	 /**
	  * Create/Get WidgetController, BindCallbacksDependencies()
	  * Set WidgetController, Call WidgetControllerSet Event (Which will assign AbilityDataDelegate)
	  */
	OverlayWidget->SetWidgetController(CreateOrGetOverlayWC(WCParams));
	OverlayWidgetController->BroadcastInitialValues();
}
