// Copyright Hung


#include "AbilitySystem/Data/AuraInputDataAsset.h"

#include "Game/AuraGameState.h"
#include "Kismet/GameplayStatics.h"

const UAuraInputDataAsset* UAuraInputDataAsset::GetFromGameState(const UObject* WorldContextObject)
{
	if (const AAuraGameState* GameState = Cast<AAuraGameState>(UGameplayStatics::GetGameState(WorldContextObject)))
	{
		return GameState->AuraInputDataAsset;
	}
	return nullptr;
}
