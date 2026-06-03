// Copyright Hung


#include "Aura/Public/AbilitySystem/AbilityTask/AbilityTask_AimData.h"

#include "AbilitySystemComponent.h"
#include "Character/AuraCharacterBase.h"
#include "Player/AuraPlayerController.h"

UAbilityTask_AimData* UAbilityTask_AimData::SendAimData(UGameplayAbility* OwningAbility)
{
	return NewAbilityTask<UAbilityTask_AimData>(OwningAbility);
}

void UAbilityTask_AimData::Activate()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC) return;
	if (IsLocallyControlled())
	{
		// Struct that is not meant to be used, automatically finish when out of scope. REQUIRED to set Prediction
		// Player input SHOULD be instantly predicted (e.g. 'hold down and charge')
		FScopedPredictionWindow(ASC, IsPredictingClient());
		const AAuraCharacterBase* Character = Cast<AAuraCharacterBase>(GetAvatarActor());
		FGameplayAbilityTargetData_AimData* Data = new FGameplayAbilityTargetData_AimData();
		Data->ActivatedTime = GetWorld()->GetTimeSeconds();
		Data->AimDirection = Character->AimDirection;
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Character->GetController()))
		{	/*Is Player Controlled*/
			Data->TargetActor = static_cast<AActor*>(PC->CursorHitEnemy);
			Data->EndPoint = PC->GetCursorHitResult().ImpactPoint;
		}
		else
		{
			Data->TargetActor = Character->CombatTarget.Get();
			Data->EndPoint = Data->TargetActor.Get() ? Character->CombatTarget->GetActorLocation() :
				GetAvatarActor()->GetActorLocation() + GetAvatarActor()->GetActorForwardVector();
		}
		FGameplayAbilityTargetDataHandle DataHandle(Data);

		if (IsPredictingClient())
		{
			ASC->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(),
				GetActivationPredictionKey(), DataHandle, FGameplayTag(),
				ASC->ScopedPredictionKey);
		}

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			ValidData.Broadcast(Data->AimDirection, Data->EndPoint, Data->TargetActor.Get());
		}
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		DelegateHandle = ASC->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(
			this, &UAbilityTask_AimData::OnTargetDataReplicatedCallback);
		if (!ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey))
		{
			SetWaitingOnRemotePlayerData(); // if data hasn't reached the server yet
		}
	}
}

void UAbilityTask_AimData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		const FGameplayAbilitySpecHandle Spec = GetAbilitySpecHandle();
		const FPredictionKey ActivationKey = GetActivationPredictionKey();
		ASC->AbilityTargetDataSetDelegate(Spec, ActivationKey).Remove(DelegateHandle);
		ASC->ConsumeClientReplicatedTargetData(Spec, ActivationKey);
	}

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

	if (ShouldBroadcastAbilityTaskDelegates()) ValidData.Broadcast(Character->AimDirection, Data->GetEndPoint(), Target);
}
