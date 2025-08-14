// Copyright Hung


#include "AbilitySystem/Abilities/AuraInputAbility.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Character/AuraCharacterBase.h"
#include "Player/AuraPlayerController.h"

void UAuraInputAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	if (AuraPlayerController == nullptr) return; // check if controlled by player
	
	GetWorld()->GetTimerManager().ClearTimer(HoldInputTimer);
	
	if (bPassHoldThreshold)
	{
		bPassHoldThreshold = false;
		HoldReleased();
	}
	else
	{
		TapReleased();
		GetWorld()->GetTimerManager().SetTimer(MultiClickTimer, this, &UAuraInputAbility::MultiClickTimePassed, HoldThreshold, false);
	}
}

void UAuraInputAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (AuraPlayerController == nullptr) return;
	
	// FTimerDelegate TimerDelegate; TimerDelegate.BindLambda([this](){bPassHoldThreshold = true;});
	// FTimerDelegate::CreateLambda([this](){bPassHoldThreshold = false;});
	// FTimerDelegate TimerDelegate; TimerDelegate.BindUFunction(this, "HoldThresholdReached", ActorInfo, TriggerEventData);
	bPassHoldThreshold = false;
	GetWorld()->GetTimerManager().SetTimer(HoldInputTimer, this, &UAuraInputAbility::HoldThresholdReached, HoldThreshold, false);
	GetWorld()->GetTimerManager().ClearTimer(MultiClickTimer);

	ClickNums++;
	if (GetCurrentAbilitySpec()->GetDynamicSpecSourceTags().HasTag(AuraGameplayTags::Input_Move_Mouse))
	{
		if (ClickNums < 2) return; // if current click has not yet reached 2
		// When Mouse moves too much
		FVector2D MousePos = FVector2D::ZeroVector;
		if (AuraPlayerController->GetMousePosition(MousePos.X, MousePos.Y))
		{
			float DistanceSquared = (MousePos - ClickScreenPosition).SizeSquared(); // Distance from previous click
			FVector2D ViewportSize = FVector2D::ZeroVector;
			if ( GEngine && GEngine->GameViewport )
			{
				// GSystemResolution.ResX; GSystemResolution.ResY;
				GEngine->GameViewport->GetViewportSize(ViewportSize);
			}
			const float Limit = FMath::Max(ViewportSize.Y / 16, 20.f);
			if (DistanceSquared < Limit * Limit)
			{
				switch (ClickNums) // Add to Switch when ActivateAbility
				{
				case 2: DoubleClick(); break;
				case 3: TripleClick(); break;
				default: break;
				}
			}
			else ClickNums = 0;
		}
		ClickScreenPosition = MousePos;
	}
	else
	{
		switch (ClickNums) // Add to Switch when ActivateAbility
		{
		case 2: DoubleClick(); break;
		case 3: TripleClick(); break;
		default: break;
		}
	}
}

void UAuraInputAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (AuraPlayerController == nullptr) return;
	
	GetWorld()->GetTimerManager().ClearTimer(HoldInputTimer);
	GetWorld()->GetTimerManager().ClearTimer(MultiClickTimer);
}

void UAuraInputAbility::HoldThresholdReached()
{
	bPassHoldThreshold = true;
	ClickNums = 0; // Cancel multiple-click when holding
}

void UAuraInputAbility::MultiClickTimePassed()
{
	ClickNums = 0;
}
