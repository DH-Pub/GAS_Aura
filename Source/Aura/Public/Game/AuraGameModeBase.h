// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UAbilityDataAsset;
class AAuraPlayerController;
class UCharacterClassDataAsset;
/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category=Default)
	const TObjectPtr<UCharacterClassDataAsset> CharacterClassData;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	UPROPERTY()
	TArray<TObjectPtr<AAuraPlayerController>> PlayerControllers;
};
