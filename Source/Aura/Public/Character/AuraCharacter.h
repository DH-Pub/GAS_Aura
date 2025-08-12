// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

class UCharacterWidgetController;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	//~ Interface =================================================================
	// Combat
	virtual int32 GetCharacterLevel_Implementation() override;
	//~ End Interface =============================================================

	UFUNCTION(BlueprintGetter)
	UCapsuleComponent* GetCameraCapsule() {return CameraCapsule;}

	UPROPERTY(VisibleAnywhere, Category=Default)
	TObjectPtr<class UNiagaraComponent> LevelUpNiagaraComponent;
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastLevelUpEffects(int32 Level);
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCameraComponent> Camera;
	UPROPERTY(VisibleAnywhere, BlueprintGetter=GetCameraCapsule)
	TObjectPtr<UCapsuleComponent> CameraCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default|Character")
	TObjectPtr<UWidgetComponent> LevelUpWidgetComponent;
	UPROPERTY()
	TObjectPtr<UCharacterWidgetController> CharacterWidgetController;
	UPROPERTY(EditAnywhere, Category = "Default|CharacterWidgetClass")
	TSubclassOf<UCharacterWidgetController> CharacterWidgetClass;
};
