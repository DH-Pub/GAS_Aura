// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AuraInputDataAsset.generated.h"

class UInputAction;
/**
 * Store Common input
 */
UCLASS()
class AURA_API UAuraInputDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category="Default|Input")
	TObjectPtr<const class UAuraInputConfig> InputConfig;
	UPROPERTY(EditDefaultsOnly, Category="Default|Input")
	TObjectPtr<const class UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Default|Input")
	TObjectPtr<const UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Default|Input")
	TObjectPtr<const UInputAction> MouseInputAction;

	static const UAuraInputDataAsset* GetFromGameState(const UObject* WorldContextObject);
};
