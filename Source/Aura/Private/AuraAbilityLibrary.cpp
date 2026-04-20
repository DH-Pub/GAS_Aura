// Copyright Hung


#include "AuraAbilityLibrary.h"

#include "KismetTraceUtils.h"
#include "Aura/Aura.h"
#include "Engine/OverlapResult.h"
#include "Interface/CombatInterface.h"
#include "PhysicsEngine/PhysicsSettings.h"

void UAuraAbilityLibrary::AddAdditionalTraceIgnoreActors(TArray<AActor*>& IgnoreActors, AActor* ActorToCompare)
{
	if (!ActorToCompare) return;
	TArray<AActor*> AttachedActors; ActorToCompare->GetAttachedActors(AttachedActors);
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

bool UAuraAbilityLibrary::TraceByChannel(const UObject* WorldContextObject, const FVector& Start, const FVector& End,
	const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& OutHits, const TArray<TEnumAsByte<ECollisionChannel>>& Channels,
	const float SweepRadius, const bool bTraceType, const bool bTraceComplex, EDrawDebugTrace::Type DrawDebugType)
{
	FCollisionQueryParams Params("ChannelTrace", bTraceComplex);
	Params.bReturnPhysicalMaterial = true; // To get PhysMaterial from hit
	Params.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable; // Face Index (not disable globally)
	Params.AddIgnoredActors(ActorsToIgnore);

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	TArray<FHitResult> HitResults;
	if (bTraceType)
	{
		TArray<FHitResult> ChannelResults; // Results by Channel, MoveTemp() will Reset() it
		for (const ECollisionChannel Channel : Channels)
		{	// Normally, we will only use ONE trace channel
			if (SweepRadius > 0.f)
			{
				World->SweepMultiByChannel(ChannelResults, Start, End, FQuat::Identity, Channel,
					FCollisionShape::MakeSphere(SweepRadius), Params);
			}
			else World->LineTraceMultiByChannel(ChannelResults, Start, End, Channel, Params);

			HitResults.Append(MoveTemp(ChannelResults));
		}
	}
	else
	{	// UCollisionProfile::Get()->ConvertToObjectType(ECC_Pawn)
		FCollisionObjectQueryParams ObjectParams;
		for (const ECollisionChannel Channel : Channels) {ObjectParams.AddObjectTypesToQuery(Channel);}
		if (SweepRadius > 0.f)
		{	// UKismetSystemLibrary::SphereTraceMultiForObjects
			World->SweepMultiByObjectType(HitResults, Start, End, FQuat::Identity, ObjectParams,
				FCollisionShape::MakeSphere(SweepRadius), Params);
		}
		else World->LineTraceMultiByObjectType(HitResults, Start, End, ObjectParams, Params);
	}

	for (FHitResult& Hit : HitResults)
	{	// Filter to prevent multiple hits on the same actor
		// a single bullet dealing damage multiple times to a single actor if using an overlap trace
		auto Pred = [&Hit](FHitResult& Other)
		{
			return Other.HitObjectHandle == Hit.HitObjectHandle;
		};
		if (!OutHits.ContainsByPredicate(Pred)) OutHits.Add(MoveTemp(Hit));
	}

#if ENABLE_DRAW_DEBUG
	DrawDebugSphereTraceMulti(World, Start, End, SweepRadius, DrawDebugType, OutHits.Num() > 0, OutHits,
		FLinearColor::Red, FLinearColor::Green, .5f);
#endif

	return OutHits.Num() > 0;
}

bool UAuraAbilityLibrary::TraceByProfile(const UObject* WorldContextObject, TArray<FHitResult>& OutHits,
	const FVector& Start, const FVector& End, const TArray<AActor*>& ActorsToIgnore,
	const FCollisionProfileName& Profile, float SweepRadius, bool bTraceComplex, EDrawDebugTrace::Type DrawDebugType)
{
	FCollisionQueryParams Params("ProfileTrace", bTraceComplex);
	Params.bReturnPhysicalMaterial = true; // To get PhysMaterial from hit
	Params.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable; // Face Index (not disable globally)
	Params.AddIgnoredActors(ActorsToIgnore);

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	TArray<FHitResult> HitResults;
	if (SweepRadius > 0.f)
	{
		World->SweepMultiByProfile(HitResults, Start, End, FQuat::Identity, Profile.Name,
			FCollisionShape::MakeSphere(SweepRadius), Params);
	}
	else World->LineTraceMultiByProfile(HitResults, Start, End, Profile.Name, Params);

	for (FHitResult& Hit : HitResults)
	{	// Filter to prevent multiple hits on the same actor
		// a single bullet dealing damage multiple times to a single actor if using an overlap trace
		auto Pred = [&Hit](FHitResult& Other)
		{
			return Other.HitObjectHandle == Hit.HitObjectHandle;
		};
		if (!OutHits.ContainsByPredicate(Pred)) OutHits.Add(MoveTemp(Hit));
	}

#if ENABLE_DRAW_DEBUG
	DrawDebugSphereTraceMulti(World, Start, End, SweepRadius, DrawDebugType, OutHits.Num() > 0, OutHits,
		FLinearColor::Red, FLinearColor::Green, .5f);
#endif

	return OutHits.Num() > 0;
}


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
