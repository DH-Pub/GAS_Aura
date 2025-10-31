// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameMode.generated.h"

class AAuraPlayerController;
class UCharacterClassDataAsset;
/**
 * Only Server have access to this
 */
UCLASS()
class AURA_API AAuraGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	UPROPERTY()
	TArray<TObjectPtr<AAuraPlayerController>> PlayerControllers;


	UPROPERTY(EditDefaultsOnly, Category=Default)
	const TObjectPtr<UCharacterClassDataAsset> CharacterClassData;
};
