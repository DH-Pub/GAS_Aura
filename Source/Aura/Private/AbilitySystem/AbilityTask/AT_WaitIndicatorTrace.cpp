// Copyright Hung


#include "AbilitySystem/AbilityTask/AT_WaitIndicatorTrace.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "AbilitySystem/Ability/TargetData/TargetActor_Indicator.h"
#include "Character/AuraCharacterBase.h"
#include "Player/AuraPlayerController.h"

UAT_WaitIndicatorTrace* UAT_WaitIndicatorTrace::WaitIndicatorTrace(class UAuraGameplayAbility* OwningAbility,
	const TSubclassOf<ATargetActor_Indicator> Class, const FIndicatorDetails Details, const bool bLocalOnly)
{
	UAT_WaitIndicatorTrace* Task = NewAbilityTask<UAT_WaitIndicatorTrace>(OwningAbility);
	Task->AuraAbility = OwningAbility;
	if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(OwningAbility->AuraCharacter->GetController()))
	{
		Task->AuraPC = PC;
	}
	Task->IndicatorClass = Class;
	Task->Details = Details;
	Task->bLocalOnly = bLocalOnly;
	return Task;
}

bool UAT_WaitIndicatorTrace::BeginSpawningActor(UAuraGameplayAbility* OwningAbility,
	const TSubclassOf<ATargetActor_Indicator> Class, ATargetActor_Indicator*& SpawnedActor)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Ability || !ASC) return false;
	const bool bIsLocalControlled = IsLocallyControlled();
	if (bIsLocalControlled || !bLocalOnly)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(OwningAbility, EGetWorldErrorMode::LogAndReturnNull))
		{	// AuraPC can be nullptr
			SpawnedActor = World->SpawnActorDeferred<ATargetActor_Indicator>(Class, FTransform::Identity,
				AuraPC, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		}
		if (SpawnedActor)
		{
			InitIndicatorActor(SpawnedActor);
			if (IsPredictingClient())
			{	// Wait for Server to spawn to replace
				IndicatorActor->OnServerSpawned.AddUObject(this, &UAT_WaitIndicatorTrace::InitIndicatorActor);
			}
			if (!bLocalOnly)
			{
				IndicatorActor->SetReplicates(true);
			}
			IndicatorActor->Details = Details;
			if (bIsLocalControlled)
			{
				ASC->GenericLocalConfirmCallbacks.AddDynamic(this, &UAT_WaitIndicatorTrace::OnConfirmCallback);
				ASC->GenericLocalCancelCallbacks.AddDynamic(this, &UAT_WaitIndicatorTrace::OnCancelCallback);
				bTickingTask = true;
			}
		}
	}

	if (!bIsLocalControlled) // If not locally controlled (server for remote client)
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		ASC->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this,
			&UAT_WaitIndicatorTrace::OnTargetDataReplicatedCallback);
		ASC->AbilityTargetDataCancelledDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this,
			&UAT_WaitIndicatorTrace::OnTargetDataReplicatedCancelledCallback);

		ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		SetWaitingOnRemotePlayerData();
	}
	return SpawnedActor != nullptr;
}

void UAT_WaitIndicatorTrace::FinishSpawningActor(UAuraGameplayAbility* OwningAbility,
	ATargetActor_Indicator* SpawnedActor)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && SpawnedActor)
	{
		check(IndicatorActor == SpawnedActor)

		const FTransform Transform = ASC->GetOwner()->GetTransform();
		SpawnedActor->FinishSpawning(Transform);
	}
}

void UAT_WaitIndicatorTrace::InitIndicatorActor(ATargetActor_Indicator* Indicator)
{
	if (IndicatorActor)
	{
		Indicator->Details = IndicatorActor->Details;
		IndicatorActor->Destroy();
	}
	IndicatorActor = Indicator;
}

void UAT_WaitIndicatorTrace::TickTask(float DeltaTime)
{	// Only for locally controlled
	Super::TickTask(DeltaTime);

	if (IndicatorActor && AuraPC)
	{
		const FVector CharacterLoc = AuraAbility->AuraCharacter->GetActorLocation();
		FVector Direction = FVector::DownVector;
		TArray<FHitResult> Results;
		AuraPC->GetHitResultsUnderCursorByProfile(Details.TraceProfile, Results);
		for (const FHitResult& Result : Results)
		{
			if (!Result.bBlockingHit) continue;
			const FVector Loc = AuraPC->GetCursorHitResult().ImpactPoint;
			VecFromCharacter = Loc - CharacterLoc;
			Direction = (Result.TraceEnd - Result.TraceStart).GetSafeNormal();
			break;
		}

		if (IsPredictingClient())
		{
			IndicatorActor->CheckAndSetLocation(CharacterLoc + VecFromCharacter, Direction);
			if (!bLocalOnly)
			{
				IndicatorActor->ServerSetLocation(CharacterLoc + VecFromCharacter, Direction);
			}
		}
		else IndicatorActor->MulticastSetLocation(CharacterLoc + VecFromCharacter, Direction);
	}
}

void UAT_WaitIndicatorTrace::OnDestroy(bool bInOwnerFinished)
{
	if (IndicatorActor)
	{
		IndicatorActor->Destroy();
	}
	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		ASC->GenericLocalConfirmCallbacks.RemoveAll(this);
		ASC->GenericLocalCancelCallbacks.RemoveAll(this);

		ASC->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), GetActivationPredictionKey()).RemoveAll(this);
		ASC->AbilityTargetDataCancelledDelegate(GetAbilitySpecHandle(), GetActivationPredictionKey()).RemoveAll(this);
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UAT_WaitIndicatorTrace::Activate()
{
	if (!IndicatorClass)
	{
		EndTask();
		return;
	}
}

void UAT_WaitIndicatorTrace::OnConfirmCallback()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Ability || !ASC || !IndicatorActor) return;

	const bool bShouldRepToServer = !Ability->GetCurrentActorInfo()->IsNetAuthority();
	FScopedPredictionWindow	ScopedPrediction(ASC, bShouldRepToServer);

	FGameplayAbilityTargetDataHandle Data = IndicatorActor->GetIndicatorDataHandle();
	FGATargetData_CommonTarget* TargetData = static_cast<FGATargetData_CommonTarget*>(Data.Get(0));
	if (IsPredictingClient())
	{
		ASC->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(),
			GetActivationPredictionKey(), Data, FGameplayTag(), ASC->ScopedPredictionKey);
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(*TargetData);
	}
	EndTask();
}

void UAT_WaitIndicatorTrace::OnCancelCallback()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC) return;

	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());
	if (IsPredictingClient())
	{
		ASC->ServerSetReplicatedTargetDataCancelled(GetAbilitySpecHandle(),
			GetActivationPredictionKey(), ASC->ScopedPredictionKey);
	}
	OnTargetDataReplicatedCancelledCallback();
}

/** Valid TargetData was replicated to use (we are server, was sent from client) */
void UAT_WaitIndicatorTrace::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data,
	FGameplayTag ActivationTag)
{
	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		ASC->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	const FGameplayAbilityTargetData* FirstData = Data.Get(0);
	if (FirstData && FirstData->GetScriptStruct() == FGATargetData_CommonTarget::StaticStruct())
	{
		const FGATargetData_CommonTarget* TargetData = static_cast<const FGATargetData_CommonTarget*>(FirstData);

		/** Server side check to make sure sent data is valid */
		if (TargetData)
		{
			const float MaxRangeAllowed = Details.MaxRange + 10.f;
			const FVector VecToLoc = TargetData->Location - Details.OriginComponent->GetComponentLocation();
			if (VecToLoc.SizeSquared2D() > MaxRangeAllowed * MaxRangeAllowed || VecToLoc.Z > MaxRangeAllowed)
			{	// Invalid Data received
				OnTargetDataReplicatedCancelledCallback();
				return;
			}
		}
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			ValidData.Broadcast(*TargetData);
		}
	}
	EndTask();
}

/** Client canceled this Targeting Task (we are the server) */
void UAT_WaitIndicatorTrace::OnTargetDataReplicatedCancelledCallback()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(FGATargetData_CommonTarget());
	}
	EndTask();
}
