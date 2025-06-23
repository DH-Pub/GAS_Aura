// Copyright Hung


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayEffectTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void UAuraSummonAbility::SetSpawnLocations()
{
	if (NumMinions == 0) return;
	const FVector Forward = AvatarActor->GetActorForwardVector();
	const FVector Location = AvatarActor->GetActorLocation();

	const float DeltaSpread = SpawnSpread / NumMinions;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	const FVector RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector);

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
		if (Hit.Distance > 0.f)
		{
			ChosenSpawnLocation = Hit.ImpactPoint;
		}
		else continue;
		SpawnLocations.Add(ChosenSpawnLocation);
	}

	// GameplayCue by defaults only send 2 RPCs each frame, needs to add array to send through EffectContextHandle 
	UAuraAbilitySystemLibrary::SetCueLocations(EffectContextHandle, SpawnLocations);
	
	FGameplayCueParameters CueParams(EffectContextHandle);
	InstigatorASC->ExecuteGameplayCue(AuraGameplayTags::GameplayCue_Summon, CueParams);
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionsClass()
{
	return MinionClasses[FMath::RandRange(0, MinionClasses.Num() - 1)];
}
