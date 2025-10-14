// Copyright Hung


#include "Aura/Public/AbilitySystem/AbilityTask/AbilityTask_AimData.h"

#include "AbilitySystemComponent.h"
#include "Character/AuraCharacterBase.h"

UAbilityTask_AimData* UAbilityTask_AimData::SendAimData(UGameplayAbility* OwningAbility)
{
	return NewAbilityTask<UAbilityTask_AimData>(OwningAbility);
}

void UAbilityTask_AimData::Activate()
{
	if (IsLocallyControlled())
	{
		SendMouseCursorData();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey)
		                      .AddUObject(this, &UAbilityTask_AimData::OnTargetDataReplicatedCallback);
		if (!AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey))
		{
			SetWaitingOnRemotePlayerData(); // if data hasn't reached the server yet
		}
	}
}

void UAbilityTask_AimData::SendMouseCursorData() const
{
	/*
	 * Struct that is not meant to be used, automatically finish when out of scope. REQUIRED to set Prediction
	 * Player input SHOULD be instantly predicted (e.g. 'hold down and charge')
	 */
	FScopedPredictionWindow(AbilitySystemComponent.Get());
	FGameplayAbilityTargetData_AimData* Data = new FGameplayAbilityTargetData_AimData();
	Data->AimDirection = Cast<AAuraCharacterBase>(GetAvatarActor())->AimDirection;
	FGameplayAbilityTargetDataHandle DataHandle(Data);

	if (IsPredictingClient())
	{
		AbilitySystemComponent->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(),
			GetActivationPredictionKey(),
			DataHandle, FGameplayTag(),AbilitySystemComponent->ScopedPredictionKey);
	}
	if (ShouldBroadcastAbilityTaskDelegates()) ValidData.Broadcast(DataHandle);
}

void UAbilityTask_AimData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
                                                           FGameplayTag ActivationTag) const
{
	AAuraCharacterBase* AuraPawn = Cast<AAuraCharacterBase>(GetAvatarActor());
	AuraPawn->AimDirection = DataHandle.Data[0]->GetEndPoint(); // Set AimDirection on Server side

	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates()) ValidData.Broadcast(DataHandle);
}
