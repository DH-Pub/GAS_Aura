// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraPlayer.generated.h"

class UCharacterWidgetController;
class UWidgetComponent;
/**
 *
 */
UCLASS()
class AURA_API AAuraPlayer : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
	AAuraPlayer();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	virtual int32 GetCharacterLevel_Implementation() const override;

	UFUNCTION(BlueprintGetter)
	UCapsuleComponent* GetCameraCapsule() {return CameraCapsule;}

	UPROPERTY()
	TObjectPtr<UCharacterWidgetController> CharacterWC;
protected:
	virtual void BeginPlay() override;
	virtual void InitAuraCharacter() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCameraComponent> Camera;
	UPROPERTY(VisibleAnywhere, BlueprintGetter=GetCameraCapsule)
	TObjectPtr<UCapsuleComponent> CameraCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> LevelUpWidgetComponent; //TODO: use AuraWorldUserWidget
	UPROPERTY(EditAnywhere, Category = "Aura|CharacterWidgetClass")
	TSubclassOf<UCharacterWidgetController> CharacterWidgetClass;
};
