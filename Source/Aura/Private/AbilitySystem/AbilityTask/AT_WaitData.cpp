// Copyright Hung


#include "AbilitySystem/AbilityTask/AT_WaitData.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"

UAT_WaitData* UAT_WaitData::SendData(class UAuraGameplayAbility* OwningAbility, const FVector& Location,
	const FVector& Direction, AActor* InActor, const float LocTolerance)
{
	UAT_WaitData* Task = NewAbilityTask<UAT_WaitData>(OwningAbility);
	Task->Location = Location;
	Task->Direction = Direction.GetSafeNormal();
	Task->Actor = InActor;
	Task->LocationTolerance = LocTolerance;
	return Task;
}

void UAT_WaitData::Activate()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC) return;
	if (IsLocallyControlled())
	{
		// Struct that is not meant to be used, automatically finish when out of scope. REQUIRED to set Prediction
		// Player input SHOULD be instantly predicted (e.g. 'hold down and charge')
		FScopedPredictionWindow(ASC, IsPredictingClient());
		FGATargetData_CommonData* Data = new FGATargetData_CommonData();
		Data->Location = Location;
		Data->Direction = Direction;
		Data->TargetActor = Actor;

		FGameplayAbilityTargetDataHandle DataHandle(Data);
		if (IsPredictingClient())
		{
			ASC->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(),
				GetActivationPredictionKey(), DataHandle, FGameplayTag(),
				ASC->ScopedPredictionKey);
		}

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnDataReceived.Broadcast(Data->Location, Data->Direction.ToOrientationRotator(), Data->TargetActor.Get());
		}
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		DelegateHandle = ASC->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(
			this, &UAT_WaitData::OnTargetDataReplicatedCallback);
		if (!ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey))
		{
			SetWaitingOnRemotePlayerData(); // if data hasn't reached the server yet
		}
	}
}

void UAT_WaitData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		const FGameplayAbilitySpecHandle Spec = GetAbilitySpecHandle();
		const FPredictionKey ActivationKey = GetActivationPredictionKey();
		ASC->AbilityTargetDataSetDelegate(Spec, ActivationKey).Remove(DelegateHandle);
		ASC->ConsumeClientReplicatedTargetData(Spec, ActivationKey);
	}

	if (const FGameplayAbilityTargetData* Data = DataHandle.Data[0].Get())
	{
		/*if (!Data || Data->GetScriptStruct() != FGATargetData_CommonData::StaticStruct()) return;
		const FGATargetData_CommonData* CommonData = static_cast<const FGATargetData_CommonData*>(Data);*/
		FTransform ClientOrigin = Data->GetOrigin();
		FVector ClientLoc = ClientOrigin.GetLocation();
		AActor* Target = Data->GetActors().Num() > 0 ? Data->GetActors()[0].Get() : nullptr;

		const float Distance = (Location - ClientLoc).SizeSquared();
		if (Distance < LocationTolerance * LocationTolerance)
		{
			Location = ClientLoc;
		}

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnDataReceived.Broadcast(Location, ClientOrigin.Rotator(), Target);
		}
	}
}
