// Copyright Hung


#include "Character/AuraMovementComponent.h"

#include "AuraAbilityLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Effect/MovementSpeedEffect.h"
#include "Character/AuraCharacterBase.h"

UAuraMovementComponent::UAuraMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true; // true for character to move
	
	// bUseControllerDesiredRotation = true;
	bOrientRotationToMovement = true;
	bConstrainToPlane = true;
	bSnapToPlaneAtStart = true;
	// bUseRVOAvoidance = true; // Avoidance
	// AvoidanceConsiderationRadius = 100.f;
}

void UAuraMovementComponent::ClientSetWalkSpeed_Implementation(const float InSpeed)
{
	MaxWalkSpeed = InSpeed;
}

void UAuraMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	// MaxWalkSpeed = BaseWalkSpeed;
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
	const FGameplayEffectSpecHandle StartSpecHandle = ASC->MakeOutgoingSpec(UStartingMovementSpeedEffect::StaticClass(), 
		1.f, ASC->MakeEffectContext());
	StartSpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(AuraGameplayTags::Attributes_Vital_MovementSpeed) = BaseWalkSpeed;
	ASC->ApplyGameplayEffectSpecToSelf(*StartSpecHandle.Data.Get());
	
	ASC->RegisterGameplayTagEvent(AuraGameplayTags::Character_State_Block_Movement,
		EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	ASC->RegisterGameplayTagEvent(AuraGameplayTags::Character_State_Block_Movement,
		EGameplayTagEventType::NewOrRemoved).AddWeakLambda(this, 
	[&](const FGameplayTag Tag, const int32 NewCount)
	{
		UAuraAbilitySystemComponent* AuraASC = AuraOwner->GetAuraAbilitySystemComponent();
		if (NewCount > 0)
		{
			StopMovementImmediately();
			const FGameplayEffectSpecHandle SpecHandle = AuraASC->MakeOutgoingSpec(UDisableMovementEffect::StaticClass(), 
				1.f, AuraASC->MakeEffectContext());
			SpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(AuraGameplayTags::Attributes_Vital_MovementSpeed) = 0.f;
			DisabledHandle = AuraASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
		else
		{
			AuraASC->RemoveActiveGameplayEffect(DisabledHandle);
		}
	});
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

