// Copyright Hung


#include "AuraAbilityLibrary.h"

#include "Character/AuraCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"

bool UAuraAbilityLibrary::YawActorToRotation(AActor* InActor, const FVector InAimDirection, const float DeltaTime,
                                             const float InterpSpeed)
{
	FRotator CurrentRot = InActor->GetActorRotation();
	FRotator TargetRot = InAimDirection.ToOrientationRotator();
	TargetRot.Pitch = TargetRot.Roll = 0.f;
	if (FMath::Abs(TargetRot.Yaw - CurrentRot.Yaw) < UE_SMALL_NUMBER) return true; // if rotation is within Tolerance

	const FRotator InterpToRot = FMath::RInterpConstantTo(CurrentRot, TargetRot, DeltaTime, InterpSpeed);
	CurrentRot.Yaw = InterpToRot.Yaw; // For up view
	InActor->GetRootComponent()->SetWorldRotation(CurrentRot);
	return false;
}

void UAuraAbilityLibrary::GetLivePlayersInRadius(const UObject* WorldContextObject, TArray<AActor*>& OutActors,
	const TArray<AActor*>& ActorsToIgnore, const float Radius, const FVector& Origin, const bool bShowDebug)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr) return;
	FCollisionQueryParams SphereParams; SphereParams.AddIgnoredActors(ActorsToIgnore);
	TArray<FOverlapResult> Overlaps;
	// UKismetSystemLibrary::SphereOverlapActors();
	World->OverlapMultiByObjectType(Overlaps, Origin, FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn)/*FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects)*/,
		FCollisionShape::MakeSphere(Radius), SphereParams);
	for (FOverlapResult& Overlap : Overlaps) OutActors.AddUnique(Overlap.GetActor());

	if (bShowDebug) UKismetSystemLibrary::DrawDebugSphere(WorldContextObject, Origin, Radius, 12, FColor::Red, 1.f);
}

bool UAuraAbilityLibrary::IsNotFriend(const AActor* FirstActor, const AActor* SecondActor)
{
	if (FirstActor == nullptr || SecondActor == nullptr) return false;
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	return !(bBothArePlayers || bBothAreEnemies);
}
