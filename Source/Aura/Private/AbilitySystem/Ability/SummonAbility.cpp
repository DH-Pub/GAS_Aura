// Copyright Hung


#include "AbilitySystem/Ability/SummonAbility.h"

#include "AbilitySystemComponent.h"
#include "AuraEffectTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Ability/DeathAbility.h"
#include "Character/AuraCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

USummonAbility::USummonAbility()
{
	ActivationOwnedTags.AddTag(AuraGameplayTags::State_Block_Movement_Speed);
	ActivationOwnedTags.AddTag(AuraGameplayTags::State_Block_Movement_Rotation);

	AuraAbilityTag = AuraGameplayTags::Ability_Summon;
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

	FGameplayCueParameters SummonCue;
	SummonCue.Instigator = AuraCharacter;
	UAbilitySystemComponent* ASC = AuraCharacter->GetAbilitySystemComponent();
	for (int32 i = 0; i < NumMinions; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * (i + 0.5) , FVector::UpVector);
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		// Prevent Spawning below ground
		FHitResult Hit; FCollisionObjectQueryParams Params(ECC_WorldStatic);
		GetWorld()->LineTraceSingleByObjectType(Hit, ChosenSpawnLocation + FVector(0.f, 0.f, 500.f),
			ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), Params);
		if (Hit.bBlockingHit)
		{
			CommitAbilityCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
			SummonLocations.Add(SummonCue.Location = Hit.ImpactPoint);
			ASC->ExecuteGameplayCue(AuraGameplayTags::GameplayCue_Shared_Summon, SummonCue);
		} // else Hit nothing (No ground) => No spawn
	}
}

void USummonAbility::PreActivate(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	SetSpawnLocations();
	// Shuffle Locations
	const int32 LastIndex = SummonLocations.Num() - 1;
	for (int32 i = 0; i <= LastIndex; ++i)
	{
		SummonLocations.Swap(i, FMath::RandRange(i, LastIndex));
	}
}

void USummonAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	SummonLocations.Reset();
	SpawnLocationIndex = 0;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USummonAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
	UDeathAbility::KillAllSummons(AuraCharacter);
}

bool USummonAbility::SpawnEnemiesByLocations()
{
	if (SpawnLocationIndex >= SummonLocations.Num()) return false;

	FVector SpawnLocation = SummonLocations[SpawnLocationIndex++];
	SpawnLocation.Z += 70.f; // Above ground
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(AuraCharacter->GetActorLocation(), SpawnLocation);
	LookAtRotation.Pitch = LookAtRotation.Roll = 0.f;
	const FTransform SpawnTransform(LookAtRotation, SpawnLocation);

	AAuraCharacterBase* SpawnedCharacter = GetWorld()->SpawnActorDeferred<AAuraCharacterBase>(GetRandomMinionsClass(),
		SpawnTransform, AuraCharacter, /*Cast<Pawn>*/AuraCharacter,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	AuraCharacter->Summons.Add(SpawnedCharacter);
	SpawnedCharacter->FinishSpawning(SpawnTransform);
	return true;
}
