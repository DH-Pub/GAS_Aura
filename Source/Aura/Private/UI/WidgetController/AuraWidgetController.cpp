// Copyright Hung


#include "UI/WidgetController/AuraWidgetController.h"

#include "Character/AuraCharacterBase.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AController* UAuraWidgetController::GetPlayerController() const {return Character->GetController();}
AAuraPlayerState* UAuraWidgetController::GetPlayerState() const {return Character->GetPlayerState<AAuraPlayerState>();}
UAuraAbilitySystemComponent* UAuraWidgetController::GetASC() const {return Character->GetAuraAbilitySystemComponent();}
UAuraAttributeSet* UAuraWidgetController::GetAttributeSet() const {return Character->GetAttributeSet();}

void UAuraWidgetController::SetCharacter(AAuraCharacterBase* InCharacter)
{
	Character = InCharacter;
	if (const AAuraPlayerController* PC = Character->GetController<AAuraPlayerController>()) {AuraHUD = PC->GetHUD<AAuraHUD>();}
}
