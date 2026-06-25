// Copyright Hung


#include "AuraAbilityLibrary.h"

#include "KismetTraceUtils.h"
#include "Aura/Aura.h"
#include "Engine/Engine.h"
#include "Engine/OverlapResult.h"
#include "Interface/CombatInterface.h"
#include "PhysicsEngine/PhysicsSettings.h"

void UAuraAbilityLibrary::AddAdditionalTraceIgnoreActors(TArray<AActor*>& IgnoreActors, AActor* ActorToCompare)
{
	if (!ActorToCompare) return;
	TArray<AActor*> AttachedActors = {ActorToCompare}; ActorToCompare->GetAttachedActors(AttachedActors);
	IgnoreActors.Append(MoveTemp(AttachedActors));

	if (AActor* Owner = ActorToCompare->GetOwner())
	{
		IgnoreActors.Add(Owner);
		TArray<AActor*> OwnerAttached; Owner->GetAttachedActors(OwnerAttached);
		IgnoreActors.Append(MoveTemp(OwnerAttached));
	}

	if (AActor* Instigator = ActorToCompare->GetInstigator())
	{
		IgnoreActors.Add(Instigator);
		TArray<AActor*> InstigatorAttached; Instigator->GetAttachedActors(InstigatorAttached);
		IgnoreActors.Append(MoveTemp(InstigatorAttached));
	}
}


#pragma region Trace______________________________________________________
void UAuraAbilityLibrary::CreateCollisionQueryParams(FCollisionQueryParams& Params, const bool bTraceComplex,
	const TArray<AActor*>& ActorsToIgnore)
{
	Params = FCollisionQueryParams("Trace", bTraceComplex);
	Params.bReturnPhysicalMaterial = true; // To get PhysMaterial from hit
	Params.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable; // Face Index (not disable globally)
	Params.AddIgnoredActors(ActorsToIgnore);
}
void UAuraAbilityLibrary::FilterHitOnSameActors(TArray<FHitResult>& OutHits, TArray<FHitResult>& HitResults)
{	// Filter to prevent multiple hits on the same actor
	for (FHitResult& Hit : HitResults)
	{	// a single bullet dealing damage multiple times to a single actor if using a trace multi
		auto Pred = [&Hit](FHitResult& Other)
		{
			return Other.HitObjectHandle == Hit.HitObjectHandle;
		};
		if (!OutHits.ContainsByPredicate(Pred)) OutHits.Add(MoveTemp(Hit));
	}
}

bool UAuraAbilityLibrary::TraceSingleByChannel(const UObject* WorldContextObject, FHitResult& OutHit,
	const FVector& Start, const FVector& End, TEnumAsByte<ECollisionChannel> Channel,
	const TArray<AActor*>& ActorsToIgnore, float SweepRadius, EDrawDebugTrace::Type DrawDebugType, bool bTraceComplex)
{
	FCollisionQueryParams Params; CreateCollisionQueryParams(Params, bTraceComplex, ActorsToIgnore);

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (SweepRadius > UE_KINDA_SMALL_NUMBER)
	{
		World->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, Channel,
			FCollisionShape::MakeSphere(SweepRadius), Params);
	}
	else World->LineTraceSingleByChannel(OutHit, Start, End, Channel, Params);

#if ENABLE_DRAW_DEBUG
	DrawDebugSphereTraceSingle(World, Start, End, SweepRadius, DrawDebugType, OutHit.bBlockingHit, OutHit,
		FLinearColor::Red, FLinearColor::Green, .5f);
#endif

	return OutHit.bBlockingHit;
}
bool UAuraAbilityLibrary::TraceMultiByChannel(const UObject* WorldContextObject, TArray<FHitResult>& OutHits,
	const FVector& Start, const FVector& End, TEnumAsByte<ECollisionChannel> Channel,
	const TArray<AActor*>& ActorsToIgnore, float SweepRadius, EDrawDebugTrace::Type DrawDebugType, bool bTraceComplex)
{
	FCollisionQueryParams Params; CreateCollisionQueryParams(Params, bTraceComplex, ActorsToIgnore);

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	TArray<FHitResult> HitResults;
	if (SweepRadius > UE_KINDA_SMALL_NUMBER)
	{
		World->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, Channel,
			FCollisionShape::MakeSphere(SweepRadius), Params);
	}
	else World->LineTraceMultiByChannel(HitResults, Start, End, Channel, Params);

	FilterHitOnSameActors(OutHits, HitResults);
#if ENABLE_DRAW_DEBUG
	DrawDebugSphereTraceMulti(World, Start, End, SweepRadius, DrawDebugType, OutHits.Num() > 0, OutHits,
		FLinearColor::Red, FLinearColor::Green, .5f);
#endif

	return OutHits.Num() > 0;
}

bool UAuraAbilityLibrary::TraceSingleByObjectType(const UObject* WorldContextObject, FHitResult& OutHit,
	const FVector& Start, const FVector& End, const TArray<TEnumAsByte<ECollisionChannel>>& Channels,
	const TArray<AActor*>& ActorsToIgnore, float SweepRadius, EDrawDebugTrace::Type DrawDebugType, bool bTraceComplex)
{
	FCollisionQueryParams Params; CreateCollisionQueryParams(Params, bTraceComplex, ActorsToIgnore);

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	FCollisionObjectQueryParams ObjectParams; // UCollisionProfile::Get()->ConvertToObjectType(ECC_Pawn)
	for (const ECollisionChannel Channel : Channels) {ObjectParams.AddObjectTypesToQuery(Channel);}
	if (SweepRadius > UE_KINDA_SMALL_NUMBER)
	{	// UKismetSystemLibrary::SphereTraceMultiForObjects
		World->SweepSingleByObjectType(OutHit, Start, End, FQuat::Identity, ObjectParams,
			FCollisionShape::MakeSphere(SweepRadius), Params);
	}
	else World->LineTraceSingleByObjectType(OutHit, Start, End, ObjectParams, Params);

#if ENABLE_DRAW_DEBUG
	DrawDebugSphereTraceSingle(World, Start, End, SweepRadius, DrawDebugType, OutHit.bBlockingHit, OutHit,
		FLinearColor::Red, FLinearColor::Green, .5f);
#endif

	return OutHit.bBlockingHit;
}

bool UAuraAbilityLibrary::TraceMultiByObjectType(const UObject* WorldContextObject, TArray<FHitResult>& OutHits,
	const FVector& Start, const FVector& End, const TArray<TEnumAsByte<ECollisionChannel>>& Channels,
	const TArray<AActor*>& ActorsToIgnore, float SweepRadius, EDrawDebugTrace::Type DrawDebugType, bool bTraceComplex)
{
	FCollisionQueryParams Params; CreateCollisionQueryParams(Params, bTraceComplex, ActorsToIgnore);

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	TArray<FHitResult> HitResults;
	FCollisionObjectQueryParams ObjectParams; // UCollisionProfile::Get()->ConvertToObjectType(ECC_Pawn)
	for (const ECollisionChannel Channel : Channels) {ObjectParams.AddObjectTypesToQuery(Channel);}
	if (SweepRadius > 0.f)
	{	// UKismetSystemLibrary::SphereTraceMultiForObjects
		World->SweepMultiByObjectType(HitResults, Start, End, FQuat::Identity, ObjectParams,
			FCollisionShape::MakeSphere(SweepRadius), Params);
	}
	else World->LineTraceMultiByObjectType(HitResults, Start, End, ObjectParams, Params);

	FilterHitOnSameActors(OutHits, HitResults);
#if ENABLE_DRAW_DEBUG
	DrawDebugSphereTraceMulti(World, Start, End, SweepRadius, DrawDebugType, OutHits.Num() > 0, OutHits,
		FLinearColor::Red, FLinearColor::Green, .5f);
#endif

	return OutHits.Num() > 0;
}

bool UAuraAbilityLibrary::TraceSingleByProfile(const UObject* WorldContextObject, FHitResult& OutHit,
	const FVector& Start, const FVector& End, const FCollisionProfileName& Profile,
	const TArray<AActor*>& ActorsToIgnore, float SweepRadius, EDrawDebugTrace::Type DrawDebugType, bool bTraceComplex)
{
	FCollisionQueryParams Params; CreateCollisionQueryParams(Params, bTraceComplex, ActorsToIgnore);

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (SweepRadius > 0.f)
	{
		World->SweepSingleByProfile(OutHit, Start, End, FQuat::Identity, Profile.Name,
			FCollisionShape::MakeSphere(SweepRadius), Params);
	}
	else World->LineTraceSingleByProfile(OutHit, Start, End, Profile.Name, Params);

#if ENABLE_DRAW_DEBUG
	DrawDebugSphereTraceSingle(World, Start, End, SweepRadius, DrawDebugType, OutHit.bBlockingHit, OutHit,
		FLinearColor::Red, FLinearColor::Green, .5f);
#endif

	return OutHit.bBlockingHit;
}
bool UAuraAbilityLibrary::TraceMultiByProfile(const UObject* WorldContextObject, TArray<FHitResult>& OutHits,
	const FVector& Start, const FVector& End, const FCollisionProfileName& Profile,
	const TArray<AActor*>& ActorsToIgnore, float SweepRadius, EDrawDebugTrace::Type DrawDebugType, bool bTraceComplex)
{
	FCollisionQueryParams Params; CreateCollisionQueryParams(Params, bTraceComplex, ActorsToIgnore);

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	TArray<FHitResult> HitResults;
	if (SweepRadius > 0.f)
	{
		World->SweepMultiByProfile(HitResults, Start, End, FQuat::Identity, Profile.Name,
			FCollisionShape::MakeSphere(SweepRadius), Params);
	}
	else World->LineTraceMultiByProfile(HitResults, Start, End, Profile.Name, Params);

	FilterHitOnSameActors(OutHits, HitResults);
#if ENABLE_DRAW_DEBUG
	DrawDebugSphereTraceMulti(World, Start, End, SweepRadius, DrawDebugType, OutHits.Num() > 0, OutHits,
		FLinearColor::Red, FLinearColor::Green, .5f);
#endif

	return OutHits.Num() > 0;
}
#pragma endregion


bool UAuraAbilityLibrary::ConeOverlapLivingCharacters(const UObject* WorldContextObject, const FVector& Start,
	FVector Direction, float SlantHeight, float ConeHalfAngleDeg, const TArray<AActor*>& ActorsToIgnore,
	EDrawDebugTrace::Type DrawDebugType, TArray<AActor*>& OutCharacters)
{
	TArray<AActor*> OverlapActors;
	GetLiveCharactersInRadius(WorldContextObject, OverlapActors, ActorsToIgnore, SlantHeight, Start,
		DrawDebugType > EDrawDebugTrace::None);

	if (!Direction.IsNormalized()) {if (Direction.Normalize()) return false;}
	const float ConeHalfAngleRad = FMath::DegreesToRadians(ConeHalfAngleDeg);
	// const float ConeBaseRadius = ConeHeight * tan(ConeHalfAngleRad); // r = h * tan(theta / 2)
	for (AActor* Actor : OverlapActors)
	{
		const FVector ActorVec = Actor->GetActorLocation() - Start;
		const float AngleRad = FMath::Acos(FVector::DotProduct(Direction, ActorVec.GetSafeNormal()));
		if (AngleRad < ConeHalfAngleRad) OutCharacters.AddUnique(Actor);
	}

#if ENABLE_DRAW_DEBUG
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		DrawDebugCone(WorldContextObject->GetWorld(), Start, Direction, SlantHeight, ConeHalfAngleRad,
			ConeHalfAngleRad, 32, FColor::Green, false, 0.05f);
	}
#endif

	return OutCharacters.Num() > 0;
}

void UAuraAbilityLibrary::SortActorsByClosest(TArray<AActor*>& Actors, const FVector& Origin)
{
	Algo::Sort(Actors, [&Origin](const AActor* A, const AActor* B) /*Actors.Sort()*/
	{return FVector::DistSquared(A->GetActorLocation(), Origin) < FVector::DistSquared(B->GetActorLocation(), Origin);});
}

bool UAuraAbilityLibrary::GetLiveCharactersInRadius(const UObject* WorldContextObject, TArray<AActor*>& OutActors,
	const TArray<AActor*>& ActorsToIgnore, const float Radius, const FVector& Origin, const bool bShowDebug)
{	// UKismetSystemLibrary::SphereOverlapActors();
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr) return false;
	FCollisionQueryParams Params; Params.AddIgnoredActors(ActorsToIgnore);
	Params.bReturnPhysicalMaterial = true; // Params.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable;

	TArray<FOverlapResult> Results;
	World->OverlapMultiByObjectType(Results, Origin, FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn)/*FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects)*/,
		FCollisionShape::MakeSphere(Radius), Params);
	for (FOverlapResult& Overlap : Results)
	{
		AActor* OverlapActor = Overlap.GetActor();
		if (OutActors.Contains(OverlapActor)) continue;
		if (OverlapActor->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(OverlapActor))
		{
			OutActors.Add(OverlapActor);
		}
	}
	if (bShowDebug) UKismetSystemLibrary::DrawDebugSphere(WorldContextObject, Origin, Radius, 12, FColor::Red, 1.f);

	return OutActors.Num() > 0;
}

bool UAuraAbilityLibrary::IsAlly(const AActor* FirstActor, const AActor* SecondActor)
{
	if (FirstActor == nullptr || SecondActor == nullptr) return false;
	const bool bBothArePlayers = FirstActor->ActorHasTag(AuraActorTag_Player) && SecondActor->ActorHasTag(AuraActorTag_Player);
	const bool bBothAreEnemies = FirstActor->ActorHasTag(AuraActorTag_Enemy) && SecondActor->ActorHasTag(AuraActorTag_Enemy);
	return bBothArePlayers || bBothAreEnemies;
}
bool UAuraAbilityLibrary::IsNotAlly(const AActor* FirstActor, const AActor* SecondActor)
{
	if (FirstActor == nullptr || SecondActor == nullptr) return false;
	const bool bBothArePlayers = FirstActor->ActorHasTag(AuraActorTag_Player) && SecondActor->ActorHasTag(AuraActorTag_Player);
	const bool bBothAreEnemies = FirstActor->ActorHasTag(AuraActorTag_Enemy) && SecondActor->ActorHasTag(AuraActorTag_Enemy);
	return !(bBothArePlayers || bBothAreEnemies);
}

void UAuraAbilityLibrary::FilterOutAllies(const AActor* InActor, TArray<AActor*>& ActorsToFilter)
{
	ActorsToFilter = ActorsToFilter.FilterByPredicate([InActor](const AActor* Actor)
	{
		return IsNotAlly(Actor, InActor);
	});
}
void UAuraAbilityLibrary::FilterOutEnemies(const AActor* InActor, TArray<AActor*>& ActorsToFilter)
{
	ActorsToFilter = ActorsToFilter.FilterByPredicate([InActor](const AActor* Actor)
	{
		return IsAlly(Actor, InActor);
	});
}
