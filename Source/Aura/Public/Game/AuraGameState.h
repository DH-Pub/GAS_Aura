// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AuraGameState.generated.h"

/**
 * All (Server & Client) has access to this
 */
UCLASS()
class AURA_API AAuraGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AAuraGameState();

	UFUNCTION(BlueprintPure)
	static void GetLivingCharactersWithTag(const UObject* WorldContextObject, const FName Tag, TArray<AActor*>& OutActors);

	virtual void Tick(float DeltaSeconds) override;
private:
	void CheckLivingCharacters();
	UPROPERTY()
	TArray<AActor*> LivingCharacters;

public:
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<const class UAbilityDataAsset> AbilityDataAsset;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<const class UAuraInputDataAsset> AuraInputDataAsset;
};
