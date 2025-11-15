// Copyright Hung


#include "AbilitySystem/AbilityTask/AbilityTask_RotateActor.h"

#include "AuraAbilityLibrary.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "Character/AuraCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UAbilityTask_RotateActor* UAbilityTask_RotateActor::WaitSpecInputPressed(class UAuraGameplayAbility* OwningAbility)
{
	UAbilityTask_RotateActor* Task = NewAbilityTask<UAbilityTask_RotateActor>(OwningAbility);
	Task->AuraCharacter = OwningAbility->AuraCharacter;
	return Task;
}

void UAbilityTask_RotateActor::Activate()
{
	Super::Activate();
	StartTime = GetWorld()->GetTimeSeconds();
}

void UAbilityTask_RotateActor::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	if (UAuraAbilityLibrary::YawActorToRotation(AuraCharacter, AuraCharacter->AimDirection, DeltaTime,
		AuraCharacter->GetCharacterMovement()->RotationRate.Yaw))
	{
		if (ShouldBroadcastAbilityTaskDelegates()) OnFinish.Broadcast(GetWorld()->GetTimeSeconds() - StartTime);
		EndTask();
	}
}
