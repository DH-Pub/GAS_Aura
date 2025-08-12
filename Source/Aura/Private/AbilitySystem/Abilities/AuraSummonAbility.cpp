// Copyright Hung


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayEffectTypes.h"
#include "AuraGameplayTags.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Character/AuraCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "StructUtils/InstancedStruct.h"

UAuraSummonAbility::UAuraSummonAbility()
{
	SetAssetTags(FGameplayTagContainer(AuraGameplayTags::Ability_Summon));
}

void UAuraSummonAbility::SetSpawnLocations()
{
	if (NumMinions == 0) return;
	const FVector Forward = AuraCharacterFromActorInfo->GetActorForwardVector();
	const FVector Location = AuraCharacterFromActorInfo->GetActorLocation();

	const float DeltaSpread = SpawnSpread / NumMinions;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);

	UAbilitySystemComponent* InstigatorASC = GetCurrentActorInfo()->AbilitySystemComponent.Get();
	FGameplayEffectContextHandle EffectContextHandle = InstigatorASC->MakeEffectContext();
	for (int32 i = 0; i < NumMinions; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * (i + 0.5) , FVector::UpVector);
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		
		FHitResult Hit;
		FCollisionObjectQueryParams Params;
		Params.AddObjectTypesToQuery(ECC_WorldStatic);
		GetWorld()->LineTraceSingleByObjectType(Hit, ChosenSpawnLocation + FVector(0.f, 0.f, 500.f),
			ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), Params);
		if (Hit.Distance > 0.f) ChosenSpawnLocation = Hit.ImpactPoint;
		else continue;
		
		// GameplayCue by defaults only send 2 RPCs each frame, needs to add array to send through EffectContextHandle 
		SummonInfo.Locations.Add(ChosenSpawnLocation);
	}
	
	const FInstancedStruct InstancedStruct = FInstancedStruct::Make(SummonInfo);
	UAuraAbilitySystemLibrary::SetInstancedStruct(EffectContextHandle, InstancedStruct);
	InstigatorASC->ExecuteGameplayCue(AuraGameplayTags::GameplayCue_Summon, FGameplayCueParameters(EffectContextHandle));
}

void UAuraSummonAbility::PreActivate(const FGameplayAbilitySpecHandle Handle,
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

void UAuraSummonAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	SummonInfo.Locations.Empty();
	SpawnLocationIndex = 0;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UAuraSummonAbility::SpawnEnemiesByLocations()
{
	if (SpawnLocationIndex >= SummonInfo.Locations.Num()) return false;
	
	FVector SpawnLocation = SummonInfo.Locations[SpawnLocationIndex++];
	SpawnLocation.Z += 70.f; // Above ground
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(AuraCharacterFromActorInfo->GetActorLocation(), SpawnLocation);
	LookAtRotation.Pitch = LookAtRotation.Roll = 0.f;
	SpawnTransform.SetRotation(LookAtRotation.Quaternion());
	
	AAuraCharacterBase* SpawnedCharacter = GetWorld()->SpawnActorDeferred<AAuraCharacterBase>(GetRandomMinionsClass(), SpawnTransform,
		AuraCharacterFromActorInfo, /*Cast<Pawn>*/AuraCharacterFromActorInfo, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	AuraCharacterFromActorInfo->AddNewSummon(SpawnedCharacter);
	SpawnedCharacter->FinishSpawning(SpawnTransform);
	return true;
}

void UAuraSummonAbility::SummonCueFromEffectContext(const UObject* WorldContextObject,
	const FGameplayEffectContextHandle& EffectContextHandle, UNiagaraSystem* Effect)
{
	if (const FInstancedStruct* InstancedStruct = UAuraAbilitySystemLibrary::GetInstancedStructPointer(EffectContextHandle))
	{
		if (const FSummonCueInfo* SummonInfo = InstancedStruct->GetPtr<FSummonCueInfo>())
		{
			for (auto& Loc : SummonInfo->Locations)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, Effect, Loc);
			}
		}
	}
}

/*bool UAuraSummonAbility::EffectContextHandleToSummonInfo(const FGameplayEffectContextHandle& EffectContextHandle,
	TArray<FVector_NetQuantize>& Locations)
{
	if (const FInstancedStruct* InstancedStruct = UAuraAbilitySystemLibrary::GetInstancedStructPointer(EffectContextHandle))
	{
		if (const FSummonCueInfo* SummonInfo = InstancedStruct->GetPtr<FSummonCueInfo>())
		{
			Locations = SummonInfo->Locations;
			return true;
		}
	}
	return false;
}*/
