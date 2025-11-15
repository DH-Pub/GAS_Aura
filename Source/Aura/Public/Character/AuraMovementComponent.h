// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AuraMovementComponent.generated.h"

/*
 * The base character movement component class used by this project
 */
UCLASS()
class AURA_API UAuraMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	UAuraMovementComponent();
	
	virtual void PostLoad() override;
	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;
	
	void SetASC(class UAuraAbilitySystemComponent* ASC);
	
	UPROPERTY(BlueprintReadWrite)
	bool bRotationTracking = false; // true if Facing Target
	
	UFUNCTION(Client, Reliable)
	void ClientSetWalkSpeed(const float InSpeed);
protected:
	virtual void BeginPlay() override;	/** Character movement component belongs to */
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<class AAuraCharacterBase> AuraOwner;
private:
	UPROPERTY(EditAnywhere, Category="Default")
	float BaseWalkSpeed = 250.f;
	
	FActiveGameplayEffectHandle DisabledHandle; 

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
