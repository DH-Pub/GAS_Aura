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
