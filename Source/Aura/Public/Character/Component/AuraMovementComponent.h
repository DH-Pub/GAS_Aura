// Copyright Hung

#pragma once

#include "CoreMinimal.h"
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

	// Called in InitializeAnimation and InitAbilityActorInfo()
	virtual void InitializeWithAbilitySystem(class UAuraAbilitySystemComponent* ASC);
protected:
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<class AAuraCharacterBase> AuraOwner;
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UAuraAbilitySystemComponent> AuraASC; // Using ASC
private:
	UPROPERTY(EditAnywhere, Category="Aura")
	float BaseWalkSpeed = 250.f;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual float GetMaxSpeed() const override;
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
};
