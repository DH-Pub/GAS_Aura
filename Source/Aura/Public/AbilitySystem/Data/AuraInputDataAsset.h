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
class AURA_API UAuraInputDataAsset : public UDataAsset /*UPrimaryDataAsset*/
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<const class UAuraInputConfig> InputConfig;
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<const class UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<const UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<const UInputAction> MouseInputAction;

	/*virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("AuraInputDataAsset", GetFName());
	}*/
};
