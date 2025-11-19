// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AuraGameState.generated.h"

/**
 * All (Server and Client) has access to this
 */
UCLASS()
class AURA_API AAuraGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AAuraGameState();

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<class UAbilityDataAsset> AbilityDataAsset;

	UFUNCTION(BlueprintPure)
	static void GetLivingCharactersWithTag(const UObject* WorldContextObject, const FName Tag, TArray<AActor*>& OutActors);

	virtual void Tick(float DeltaSeconds) override;
private:
	void CheckLivingCharacters();
	UPROPERTY()
	TArray<AActor*> LivingCharacters;
};
