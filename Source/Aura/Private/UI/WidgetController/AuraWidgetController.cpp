// Copyright Hung


#include "UI/WidgetController/AuraWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Character/AuraCharacterBase.h"
#include "Player/AuraPlayerState.h"

// AController* UAuraWidgetController::GetPlayerController() const {return Character->GetController();}
const UAuraAttributeSet* UAuraWidgetController::GetAttributeSet() const
{
	return AuraASC ? AuraASC->GetSet<UAuraAttributeSet>() : nullptr;
}

AAuraPlayerState* UAuraWidgetController::GetPlayerState() const
{
	return AuraASC ? Cast<AAuraPlayerState>(AuraASC->GetOwnerActor()) : nullptr;
}

AAuraCharacterBase* UAuraWidgetController::GetAuraCharacter() const
{
	return AuraASC ? Cast<AAuraCharacterBase>(AuraASC->GetAvatarActor()) : nullptr;
}
