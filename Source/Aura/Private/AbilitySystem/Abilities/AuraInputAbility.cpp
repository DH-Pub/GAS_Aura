// Copyright Hung


#include "AbilitySystem/Abilities/AuraInputAbility.h"

#include "InputTriggers.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Input/AuraInputComponent.h"
#include "Player/AuraPlayerController.h"

UAuraInputAbility::UAuraInputAbility()
{
	FGameplayTagContainer DefaultAssetTags;
	SetAssetTags(AddGenericAssetTags(DefaultAssetTags));
	SetGenericCancelBlockAbility();
	bRetriggerInstancedAbility = true;
}

void UAuraInputAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	//TODO: Find sth to do with this
	if (AbilityTriggerEvent == ETriggerEvent::Canceled) TapReleased();
	else if (AbilityTriggerEvent == ETriggerEvent::Completed) HoldReleased();
}

void UAuraInputAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (AbilityTriggerEvent == ETriggerEvent::Started) StartPressedOngoing();
	
	/*GEngine->AddOnScreenDebugMessage(100, 1.f, FColor::Cyan,
		FString::Printf(TEXT("%s"), *UEnum::GetValueAsString(AbilityTriggerEvent)));*/
}


void UAuraInputAbility::StartPressedOngoing_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(RepeatDelayTimer);
	switch (++ClickNums)
	{
	case 2: DoubleClick(); break;
	case 3: TripleClick(); break;
	default: break;
	}
	if (ClickNums >= MaxRepeatedClick) {ClickNums = 0;}
	
	// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green,FString("Start"));
}
void UAuraInputAbility::StartHoldTriggered_Implementation()
{
	// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow,FString("Hold Triggered"));
}

void UAuraInputAbility::TapReleased_Implementation()
{
	// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan,FString("Tap"));
}
void UAuraInputAbility::HoldReleased_Implementation()
{
	// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan,FString("Hold Released"));
}

void UAuraInputAbility::DoubleClick_Implementation()
{
	// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Magenta,FString("Double Click"));
}
void UAuraInputAbility::TripleClick_Implementation()
{
	// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Magenta,FString("Triple Click"));
}


void UAuraInputAbility::SetAbilityTriggerEvent(ETriggerEvent TriggerEvent)
{
	if (AbilityTriggerEvent == TriggerEvent) return;
	
	AbilityTriggerEvent = TriggerEvent;
	switch (TriggerEvent)
	{
	case ETriggerEvent::Triggered:
		ClickNums = 0;
		StartHoldTriggered();
		break;
	case ETriggerEvent::Canceled:
		// FTimerDelegate TimerDelegate; TimerDelegate.BindLambda([this](){AbilityTriggerEvent = ETriggerEvent::Ongoing;});
		// FTimerDelegate TimerDelegate; TimerDelegate.BindUFunction(this, "HoldThresholdReached", ActorInfo, TriggerEventData);
		GetWorld()->GetTimerManager().SetTimer(RepeatDelayTimer, FTimerDelegate::CreateLambda([this]
		{
			ClickNums = 0;
		}),  RepeatDelayTime, false);
		break;
	default: break;
	}

	// Debug
	/*GEngine->AddOnScreenDebugMessage(200, 1.f, FColor::Yellow,
		FString::Printf(TEXT("%s"), *UEnum::GetValueAsString(AbilityTriggerEvent)));*/
}

FInputActionValue UAuraInputAbility::GetBoundAuraActionValue()
{
	if (AuraPlayerController) return AuraPlayerController->AuraInputComponent->BindActionValue(InputAction).GetValue();
	return FInputActionValue();
}
