// Copyright Hung


#include "AbilitySystem/AbilityTask/AbilityTask_RotateActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "AbilitySystem/Ability/TargetData/TargetActor_Indicator.h"
#include "Character/AuraCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"

UAbilityTask_RotateActor* UAbilityTask_RotateActor::WaitRotateToTarget(UAuraGameplayAbility* OwningAbility,
	const float Multiplier)
{
	UAbilityTask_RotateActor* Task = NewAbilityTask<UAbilityTask_RotateActor>(OwningAbility);
	Task->AuraCharacter = OwningAbility->AuraCharacter;
	Task->Multiplier = Multiplier;
	return Task;
}

void UAbilityTask_RotateActor::TickTask(float DeltaTime)
{	// Super::TickTask(DeltaTime); // Empty
	const FRotator CurrentRot = AuraCharacter->GetActorRotation();
	FRotator DesiredRot(CurrentRot.Pitch, TargetRotation.Yaw, CurrentRot.Roll);
	if (FMath::Abs(DesiredRot.Yaw - CurrentRot.Yaw) > 1.f)
	{	// Clamp DesiredRot by DeltaRotation
		DesiredRot.Yaw = FMath::FixedTurn(CurrentRot.Yaw, DesiredRot.Yaw,
			GetAxisDeltaRot(RotationRate.Yaw * Multiplier, DeltaTime));
	}
	AuraCharacter->GetCharacterMovement()->MoveUpdatedComponent(FVector::ZeroVector, DesiredRot, false);
	if (FMath::Abs(AuraCharacter->GetActorRotation().Yaw - TargetRotation.Yaw) <  SCENECOMPONENT_ROTATOR_TOLERANCE)
	{
		if (ShouldBroadcastAbilityTaskDelegates()) OnFinish.Broadcast(Direction, TargetActor, GetWorld()->GetTimeSeconds() - StartTime);
		EndTask();
	}
}

void UAbilityTask_RotateActor::Activate()
{
	Super::Activate();
	StartTime = GetWorld()->GetTimeSeconds();

	Direction = AuraCharacter->AimDirection;
	if (!Direction.IsNormalized()) Direction.Normalize();
	TargetRotation = Direction.ToOrientationRotator();
	RotationRate = AuraCharacter->GetCharacterMovement()->RotationRate;

	const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
	const FPredictionKey PredictionKey = GetActivationPredictionKey();
	if (IsLocallyControlled())
	{
		const AAuraPlayerController* PC = Cast<AAuraPlayerController>(AuraCharacter->GetController());
		TargetActor = PC /*IsPlayer*/ ? static_cast<AActor*>(PC->CursorHitEnemy) : AuraCharacter->CombatTarget.Get();

		FScopedPredictionWindow(AbilitySystemComponent.Get(), IsPredictingClient());
		FGATargetData_CommonTarget* Data = new FGATargetData_CommonTarget();
		Data->TargetActor = TargetActor;
		Data->Direction = Direction;

		if (IsPredictingClient())
		{
			AbilitySystemComponent->CallServerSetReplicatedTargetData(SpecHandle,
				PredictionKey, FGameplayAbilityTargetDataHandle(Data), FGameplayTag(),
				AbilitySystemComponent->ScopedPredictionKey);
		}
		bTickingTask = true;
	}
	else
	{
		DelegateHandle = AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, PredictionKey).AddUObject(
			this, &UAbilityTask_RotateActor::OnTargetDataReplicatedCallback);
		if (!AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, PredictionKey))
		{
			SetWaitingOnRemotePlayerData(); // if data hasn't reached the server yet
		}
	}
}

void UAbilityTask_RotateActor::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	if (!AbilitySystemComponent.IsValid()) return;
	AbilitySystemComponent->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(
		DelegateHandle);
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	const FGameplayAbilityTargetData* Data = DataHandle.Data[0].Get();
	if (!Data || Data->GetScriptStruct() != FGATargetData_CommonTarget::StaticStruct()) return;
	const FGATargetData_CommonTarget* CommonTarget = static_cast<const FGATargetData_CommonTarget*>(Data);
	Direction = CommonTarget->Direction; // Set AimDirection on Server side
	TargetRotation = Direction.ToOrientationRotator();
	bTickingTask = true;
}
