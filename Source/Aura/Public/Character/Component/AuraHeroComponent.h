// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AuraHeroComponent.generated.h"

class AAuraCharacterBase;
struct FInputActionValue;

/**
 * Component that sets up Input and camera handling for player controlled pawn (or bots that simulate players).
 * //TODO: This is unfinished, complete it
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraHeroComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UAuraHeroComponent();

	virtual void BeginPlay() override;

	void SetAuraHeroInputComponent(class UAuraInputComponent* AuraInputComponent);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<const class UAuraInputDataAsset> InputDataAsset;

private:
	AAuraCharacterBase* GetAuraCharacter(); // Set AuraCharacter & AuraASC if not
	UPROPERTY()
	TObjectPtr<AAuraCharacterBase> AuraCharacter;
	UPROPERTY()
	TObjectPtr<class UAuraAbilitySystemComponent> AuraASC;

	void Move(const FInputActionValue& InputActionValue);

	void AbilityPressed(const uint8 InputID);
	void AbilityReleased(const uint8 InputID);

	bool bASCInputBound = false;
};
