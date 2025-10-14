// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AuraAssetManager.generated.h"

/**
 * This is to load old AuraGameplayTags from the course inside StartInitialLoading()
 * 
 * Project Settings -> Engine -> General -> Default Classes -> Advanced -> Asset Manager Class
 * 
 * in Config/DefaultEngine.ini will show the following
 * [/Script/Engine.Engine]
 * AssetManagerClassName=/Script/Aura.AuraAssetManager
 */
UCLASS()
class UE_DEPRECATED(5.4, "No use for this") AURA_API UAuraAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UAuraAssetManager& Get();
protected:
	virtual void StartInitialLoading() override;
};
