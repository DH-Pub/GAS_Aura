// Copyright Hung


#include "Character/Component/AuraMovementComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Character/AuraCharacterBase.h"

UAuraMovementComponent::UAuraMovementComponent()
{	// default UMovementComponent PrimaryComponentTick.bCanEverTick = true; (for character to move)
	// bUseControllerDesiredRotation = true;
	bOrientRotationToMovement = true;
	bConstrainToPlane = true;
	bSnapToPlaneAtStart = true;
	// bUseRVOAvoidance = true; // Avoidance
	// AvoidanceConsiderationRadius = 100.f;

	/*NetworkMaxSmoothUpdateDistance = 92.f;
	NetworkNoSmoothUpdateDistance = 140.f;*/
}

void UAuraMovementComponent::PostLoad()
{
	Super::PostLoad();
	AuraOwner = Cast<AAuraCharacterBase>(CharacterOwner);
}
void UAuraMovementComponent::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	Super::SetUpdatedComponent(NewUpdatedComponent);
	AuraOwner = Cast<AAuraCharacterBase>(CharacterOwner);
}

void UAuraMovementComponent::InitializeWithAbilitySystem(UAuraAbilitySystemComponent* ASC)
{
	AuraASC = ASC;
	if (!AuraOwner) return;
	if (const UAuraAttributeSet* AuraAS = AuraOwner->GetAttributeSet())
	{
		MaxWalkSpeed = FMath::Max(BaseWalkSpeed + AuraAS->GetMovementSpeed(), 0.f);
		FOnGameplayAttributeValueChange& OnSpeedChanged = AuraASC->GetGameplayAttributeValueChangeDelegate(
			AuraAS->GetMovementSpeedAttribute());
		OnSpeedChanged.RemoveAll(this); // OnSpeedChanged.Clear();
		OnSpeedChanged.AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
		{
			MaxWalkSpeed = FMath::Max(BaseWalkSpeed + Data.NewValue, 0.f);
		});
	}
}

void UAuraMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (AuraOwner && AuraOwner->bTracking)
	{	// UCharacterMovementComponent::PhysicsRotation
		const FRotator CurrentRot = UpdatedComponent->GetComponentRotation();
		const FRotator DeltaRot = Super::GetDeltaRotation(DeltaTime) * 3.f; // Increase Rotation for Aiming
		FRotator DesiredRot = AuraOwner->AimDirection.ToOrientationRotator();
		DesiredRot.Pitch = CurrentRot.Pitch; DesiredRot.Roll = CurrentRot.Roll;
		if (FMath::Abs(DesiredRot.Yaw - CurrentRot.Yaw) > 1.f)
		{	// Clamp DesiredRot by DeltaRotation
			DesiredRot.Yaw = FMath::FixedTurn(CurrentRot.Yaw, DesiredRot.Yaw, DeltaRot.Yaw);
		}
		MoveUpdatedComponentImpl(FVector(), DesiredRot.Quaternion(), false);//MoveUpdatedComponent
	}
}

float UAuraMovementComponent::GetMaxSpeed() const
{
	if (AuraASC && AuraASC->HasMatchingGameplayTag(AuraGameplayTags::State_Block_Movement_Speed))
	{
		return 0;
	}
	return Super::GetMaxSpeed();
}

FRotator UAuraMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	if (AuraASC && AuraASC->HasMatchingGameplayTag(AuraGameplayTags::State_Block_Movement_Rotation))
	{
		return FRotator();
	}
	return Super::GetDeltaRotation(DeltaTime);
}

void UAuraMovementComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);
	if (AuraOwner && AuraOwner->GetAbilitySystemComponent())
	{
		AuraOwner->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
			AuraOwner->GetAttributeSet()->GetMovementSpeedAttribute()).RemoveAll(this);
	}
}
