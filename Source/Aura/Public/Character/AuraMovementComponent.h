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

	void SetASC(class UAuraAbilitySystemComponent* ASC);

	UPROPERTY(BlueprintReadWrite)
	bool bRotationTracking = false; // true if Facing Target
protected:
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<class AAuraCharacterBase> AuraOwner;
private:
	UPROPERTY(EditAnywhere, Category="Default")
	float BaseWalkSpeed = 250.f;

	FDelegateHandle MoveSpeedDelegate;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual float GetMaxSpeed() const override;
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
};
