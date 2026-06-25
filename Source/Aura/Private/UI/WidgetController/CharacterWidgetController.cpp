// Copyright Hung


#include "UI/WidgetController/CharacterWidgetController.h"

#include "Player/AuraPlayerState.h"

void UCharacterWidgetController::UnbindOldAbilitySystemComponent()
{
	if (AAuraPlayerState* PS = GetPlayerState())
	{
		PS->OnLevelChangedDelegate.RemoveAll(this);
	}
}

void UCharacterWidgetController::BindCallbacksDependencies()
{
	if (AAuraPlayerState* PS = GetPlayerState())
	{
		PS->OnLevelChangedDelegate.AddWeakLambda(this, [this](int32 NewLevel)
		{
			// OnLevelUpDelegate.Broadcast(NewLevel);
		});
	}
}
