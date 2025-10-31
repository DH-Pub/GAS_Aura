// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AuraAssetManager.generated.h"

/**
 * //TODO: Delete this someday
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
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	static UAuraAssetManager& Get() {check(GEngine); return *Cast<UAuraAssetManager>(GEngine->AssetManager);}
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
protected:
	virtual void StartInitialLoading() override {Super::StartInitialLoading();}
};
