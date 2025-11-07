// Copyright Hung


#include "AbilitySystem/Ability/SummonAbility.h"

#include "AbilitySystemComponent.h"
#include "AuraEffectTypes.h"
#include "AuraGameplayTags.h"
#include "GameplayCue_Types.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/AuraCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

USummonAbility::USummonAbility()
{
	FGameplayTagContainer AssetTags(AuraGameplayTags::Ability_Summon);
	SetAssetTags(AddGenericAssetTags(AssetTags));
	bStopRotation = bStopMovement = true;
}

bool USummonAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	const bool bLessSummonsThanThreshold = AuraCharacter->Summons.Num() < AuraCharacter->SummonSpawnThreshold;
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) && bLessSummonsThanThreshold;
}

void USummonAbility::SetSpawnLocations()
{
	if (NumMinions == 0) return;
	const FVector Forward = AuraCharacter->GetActorForwardVector();
	const FVector Location = AuraCharacter->GetActorLocation();

	const float DeltaSpread = SpawnSpread / NumMinions;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);

	for (int32 i = 0; i < NumMinions; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * (i + 0.5) , FVector::UpVector);
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		// Prevent Spawning below ground
		FHitResult Hit; FCollisionObjectQueryParams Params(ECC_WorldStatic);
		GetWorld()->LineTraceSingleByObjectType(Hit, ChosenSpawnLocation + FVector(0.f, 0.f, 500.f),
			ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), Params);
		if (Hit.Distance > UE_KINDA_SMALL_NUMBER)
		{
			ChosenSpawnLocation = Hit.ImpactPoint;
			SummonInfo.Locations.Add(ChosenSpawnLocation); // GC defaults only send 2 RPCs per frame, use context
		} // else Hit nothing (No ground) => No spawn
	}

	const FGameplayEffectContextHandle EffectContextHandle = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
	FAuraEffectContext::ExtractAuraContext(EffectContextHandle)->SetInstancedStruct(FInstancedStruct::Make(SummonInfo));
	UAbilitySystemComponent* InstigatorASC = GetAbilitySystemComponentFromActorInfo();
	InstigatorASC->ExecuteGameplayCue(AuraGameplayTags::GameplayCue_Summon, EffectContextHandle);
}

void USummonAbility::PreActivate(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	SetSpawnLocations();
	// Shuffle Locations
	const int32 LastIndex = SummonInfo.Locations.Num() - 1;
	for (int32 i = 0; i <= LastIndex; ++i)
	{
		const int32 Index = FMath::RandRange(i, LastIndex);
		SummonInfo.Locations.Swap(i, Index);
	}
}

void USummonAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	SummonInfo.Locations.Reset();
	SpawnLocationIndex = 0;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool USummonAbility::SpawnEnemiesByLocations()
{
	if (SpawnLocationIndex >= SummonInfo.Locations.Num()) return false;

	FVector SpawnLocation = SummonInfo.Locations[SpawnLocationIndex++];
	SpawnLocation.Z += 70.f; // Above ground
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(AuraCharacter->GetActorLocation(), SpawnLocation);
	LookAtRotation.Pitch = LookAtRotation.Roll = 0.f;
	SpawnTransform.SetRotation(LookAtRotation.Quaternion());

	AAuraCharacterBase* SpawnedCharacter = GetWorld()->SpawnActorDeferred<AAuraCharacterBase>(GetRandomMinionsClass(), SpawnTransform,
		AuraCharacter, /*Cast<Pawn>*/AuraCharacter, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	AuraCharacter->AddNewSummon(SpawnedCharacter);
	SpawnedCharacter->FinishSpawning(SpawnTransform);
	return true;
}

void USummonAbility::SummonCueFromEffectContext(const FGameplayCueParameters& Parameters, UNiagaraSystem* Effect)
{
	if (const FSummonCueInfo* SummonInfo = FAuraEffectContext::GetContextStruct<FSummonCueInfo>(Parameters.EffectContext))
	{
		for (auto& Loc : SummonInfo->Locations)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(Parameters.GetInstigator(), Effect, Loc);
		}
	}
}
