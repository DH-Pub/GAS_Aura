// Copyright Hung


#include "AbilitySystem/Abilities/AuraInputAbility.h"

#include "AuraGameplayTags.h"
#include "InputTriggers.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerController.h"

UAuraInputAbility::UAuraInputAbility()
{
	bRetriggerInstancedAbility = true;
}

void UAuraInputAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	// TODO: Find sth to do with this
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
}
void UAuraInputAbility::StartHoldTriggered_Implementation()
{
	ClickNums = 0;
}


void UAuraInputAbility::TapReleased_Implementation()
{
	// FTimerDelegate TimerDelegate; TimerDelegate.BindLambda([this](){AbilityTriggerEvent = ETriggerEvent::Ongoing;});
	// FTimerDelegate TimerDelegate; TimerDelegate.BindUFunction(this, "HoldThresholdReached", ActorInfo, TriggerEventData);
	GetWorld()->GetTimerManager().SetTimer(RepeatDelayTimer, FTimerDelegate::CreateLambda([this]
	{
		ClickNums = 0;
	}),  RepeatDelayTime, false);
}
void UAuraInputAbility::HoldReleased_Implementation()
{
}


void UAuraInputAbility::DoubleClick_Implementation()
{
}
void UAuraInputAbility::TripleClick_Implementation()
{
}


void UAuraInputAbility::SetAbilityTriggerEvent(ETriggerEvent TriggerEvent)
{
	if (AbilityTriggerEvent == TriggerEvent) return;
	
	AbilityTriggerEvent = TriggerEvent;
	switch (TriggerEvent)
	{
	case ETriggerEvent::Ongoing: StartPressedOngoing(); break;
	case ETriggerEvent::Triggered: StartHoldTriggered(); break;

	case ETriggerEvent::Started: break;
	case ETriggerEvent::Canceled: TapReleased(); break;
	case ETriggerEvent::Completed: HoldReleased(); break;
		
	case ETriggerEvent::None: break;
	}
}
