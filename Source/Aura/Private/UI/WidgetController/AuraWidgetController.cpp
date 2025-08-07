// Copyright Hung


#include "UI/WidgetController/AuraWidgetController.h"

#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

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
}
