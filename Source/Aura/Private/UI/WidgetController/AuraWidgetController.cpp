// Copyright Hung


#include "UI/WidgetController/AuraWidgetController.h"

#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

FWidgetControllerParams::FWidgetControllerParams(AAuraPlayerController* PC)
{
	PlayerController = PC;
	PlayerState = PlayerController->GetPlayerState<AAuraPlayerState>();
	AbilitySystemComponent = PlayerState->GetAuraAbilitySystemComponent();
	AttributeSet = PlayerState->GetAuraAttributeSet();
}

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
	if (PlayerController) AuraHUD = PlayerController->GetHUD<AAuraHUD>();
}
