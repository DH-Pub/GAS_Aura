// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/GameStateBase.h"
#include "AuraGameState.generated.h"

class UAuraAbilitySystemComponent;
/**
 * All (Server & Client) has access to this
 */
UCLASS()
class AURA_API AAuraGameState : public AGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraGameState();
	static AAuraGameState* Get(const UObject* WorldContextObject);

	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface
	UFUNCTION(BlueprintCallable)
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const {return AbilitySystemComponent;}

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostInitializeComponents() override;

	// virtual void AddPlayerState(APlayerState* PlayerState) override;
	// virtual void RemovePlayerState(APlayerState* PlayerState) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure)
	static void GetLivingCharactersWithTag(const UObject* WorldContextObject, const FName Tag, TArray<AActor*>& OutActors);

	UPROPERTY(Replicated)
	float ServerFPS;
protected:

private:
	//TODO: Use this to apply World GE, Abilities, and GameplayCues
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TArray<TSubclassOf<class UAuraGameplayAbility>> LevelAbilities;

	void CheckLivingCharacters();
	UPROPERTY()
	TArray<TObjectPtr<AActor>> LivingCharacters;

public:
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<const class UAbilityDataAsset> AbilityDataAsset;
};
