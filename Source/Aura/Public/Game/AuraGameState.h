// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AuraGameState.generated.h"

class UAbilityDataAsset;
/**
 * All (Server and Client) has access to this
 */
UCLASS()
class AURA_API AAuraGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<UAbilityDataAsset> AbilityDataAsset;
};
