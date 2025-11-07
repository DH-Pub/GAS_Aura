// Copyright Hung


#include "Aura/Public/AbilitySystem/AbilityTask/AbilityTask_AimData.h"

#include "AbilitySystemComponent.h"
#include "Character/AuraCharacterBase.h"
#include "GameFramework/GameStateBase.h"
#include "Player/AuraPlayerController.h"

UAbilityTask_AimData* UAbilityTask_AimData::SendAimData(UGameplayAbility* OwningAbility)
{
	return NewAbilityTask<UAbilityTask_AimData>(OwningAbility);
}

void UAbilityTask_AimData::Activate()
{
	if (IsLocallyControlled())
	{
		// Struct that is not meant to be used, automatically finish when out of scope. REQUIRED to set Prediction
		// Player input SHOULD be instantly predicted (e.g. 'hold down and charge')
		FScopedPredictionWindow(AbilitySystemComponent.Get(), IsPredictingClient());
		const AAuraCharacterBase* Character = Cast<AAuraCharacterBase>(GetAvatarActor());
		FGameplayAbilityTargetData_AimData* Data = new FGameplayAbilityTargetData_AimData();
		Data->ActivatedTime = GetWorld()->GetTimeSeconds();
		Data->AimDirection = Character->AimDirection;
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Character->GetController()))
		{	/*PlayerControlled*/
			Data->TargetActor = static_cast<AActor*>(PC->CursorHitEnemy);
			Data->EndPoint = PC->GetCursorHitResult().ImpactPoint;
		}
		else
		{
			Data->TargetActor = Character->CombatTarget.Get();
			Data->bHasEndPoint = Data->TargetActor.IsValid();
			if (Data->bHasEndPoint) Data->EndPoint = Character->CombatTarget->GetActorLocation();
		}
		FGameplayAbilityTargetDataHandle DataHandle(Data);

		if (IsPredictingClient())
		{
			AbilitySystemComponent->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(),
				GetActivationPredictionKey(), DataHandle, FGameplayTag(),
				AbilitySystemComponent->ScopedPredictionKey);
		}
		if (ShouldBroadcastAbilityTaskDelegates()) ValidData.Broadcast(Data->EndPoint, /*DataHandle*/Data->TargetActor.Get());
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		DelegateHandle = AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(
			this, &UAbilityTask_AimData::OnTargetDataReplicatedCallback);
		if (!AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey))
		{
			SetWaitingOnRemotePlayerData(); // if data hasn't reached the server yet
		}
	}
}

void UAbilityTask_AimData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	AAuraCharacterBase* Character = Cast<AAuraCharacterBase>(GetAvatarActor());
	const FGameplayAbilityTargetData* Data = DataHandle.Data[0].Get();
	const FTransform Origin = Data->GetOrigin();
	Character->AimDirection = Origin.GetTranslation(); // Set AimDirection on Server side
	const FVector Floats = Origin.GetScale3D();

	const float TimeDifferent = GetWorld()->GetTimeSeconds() - Floats.X; // Server-Client
	if (TimeDifferent < 1.f && TimeDifferent > UE_KINDA_SMALL_NUMBER)
	{
		// can calculate different compared to client's time here
	}

	AActor* Target = Data->GetActors().Num() > 0 ? Data->GetActors()[0].Get() : nullptr;

	AbilitySystemComponent->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates()) ValidData.Broadcast(Data->GetEndPoint(), Target);
}
