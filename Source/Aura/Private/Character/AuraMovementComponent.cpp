// Copyright Hung


#include "Character/AuraMovementComponent.h"

#include "AuraAbilityLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Effect/MovementSpeedEffect.h"
#include "Character/AuraCharacterBase.h"

UAuraMovementComponent::UAuraMovementComponent()
{	// default UMovementComponent PrimaryComponentTick.bCanEverTick = true; (for character to move)
	// bUseControllerDesiredRotation = true;
	bOrientRotationToMovement = true;
	bConstrainToPlane = true;
	bSnapToPlaneAtStart = true;
	// bUseRVOAvoidance = true; // Avoidance
	// AvoidanceConsiderationRadius = 100.f;
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

void UAuraMovementComponent::SetASC(UAuraAbilitySystemComponent* ASC)
{
	if (MoveSpeedDelegate.IsValid())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(AuraOwner->GetAttributeSet()->GetMovementSpeedAttribute()).Remove(MoveSpeedDelegate);
	}
	MoveSpeedDelegate = ASC->GetGameplayAttributeValueChangeDelegate(
		AuraOwner->GetAttributeSet()->GetMovementSpeedAttribute()).AddWeakLambda(this,
	[&](const FOnAttributeChangeData& Data)
	{
		MaxWalkSpeed = FMath::Max(Data.NewValue, 0.f);
	});

	const FGameplayEffectSpecHandle StartSpecHandle = ASC->MakeOutgoingSpec(UStartingMovementSpeedEffect::StaticClass(),
		1.f, ASC->MakeEffectContext());
	StartSpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(AuraGameplayTags::Attributes_Vital_MovementSpeed) = BaseWalkSpeed;
	ASC->ApplyGameplayEffectSpecToSelf(*StartSpecHandle.Data.Get());
}

void UAuraMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bRotationTracking)
	{
		UAuraAbilityLibrary::YawActorToRotation(AuraOwner, AuraOwner->AimDirection, DeltaTime,
			AuraOwner->BaseRotationRate.Yaw);
	}
}

float UAuraMovementComponent::GetMaxSpeed() const
{
	if (AuraOwner && AuraOwner->GetAbilitySystemComponent() &&
		AuraOwner->GetAbilitySystemComponent()->HasMatchingGameplayTag(AuraGameplayTags::Character_State_Block_Movement))
	{
		return 0;
	}
	return Super::GetMaxSpeed();
}

FRotator UAuraMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	if (AuraOwner && AuraOwner->GetAbilitySystemComponent() &&
		AuraOwner->GetAbilitySystemComponent()->HasMatchingGameplayTag(AuraGameplayTags::Character_State_Block_Movement))
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
			AuraOwner->GetAttributeSet()->GetMovementSpeedAttribute()).Remove(MoveSpeedDelegate);
	}
}
