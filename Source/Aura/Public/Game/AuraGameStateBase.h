// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AuraGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<class UAbilityDataAsset> AbilityData;
};
