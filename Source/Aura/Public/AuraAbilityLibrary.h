// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h" // EDrawDebugTrace
#include "AuraAbilityLibrary.generated.h"

enum class ECharacterClass : uint8;
class UAbilitySystemComponent;
/**
 * For calling static Blueprint Functions
 */
UCLASS()
class AURA_API UAuraAbilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void AddAdditionalTraceIgnoreActors(TArray<AActor*>& IgnoreActors, AActor* ActorToCompare);


#pragma region Trace______________________________________________________
	static void CreateCollisionQueryParams(FCollisionQueryParams& Params, const bool bTraceComplex,
		const TArray<AActor*>& ActorsToIgnore);
	static void FilterHitOnSameActors(TArray<FHitResult>& OutHits, TArray<FHitResult>& HitResults);

	/** UKismetSystemLibrary::LineTrace SphereTrace
	 * @param Channel bTraceType HAS TO BE true (NOT Object Type)
	 * @param SweepRadius == 0.f: Line Trace, > 0.f: Sphere Sweep
	 */
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraLibrary")
	static bool TraceSingleByChannel(const UObject* WorldContextObject, FHitResult& OutHit, const FVector& Start,
		const FVector& End, TEnumAsByte<ECollisionChannel> Channel, const TArray<AActor*>& ActorsToIgnore,
		float SweepRadius = 0.f, EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None, bool bTraceComplex = false);
	/** UKismetSystemLibrary::LineTrace SphereTrace
	 * @param Channel bTraceType HAS TO BE true (NOT Object Type)
	 * @param SweepRadius == 0.f: Line Trace, > 0.f: Sphere Sweep
	 */
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraLibrary")
	static bool TraceMultiByChannel(const UObject* WorldContextObject, TArray<FHitResult>& OutHits, const FVector& Start,
		const FVector& End, TEnumAsByte<ECollisionChannel> Channel, const TArray<AActor*>& ActorsToIgnore,
		float SweepRadius = 0.f, EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None, bool bTraceComplex = false);

	/** UKismetSystemLibrary::LineTrace / SphereTrace ForObjects
	 * @param Channels bTraceType HAS TO BE false for these channels
	 * @param SweepRadius == 0.f: Line Trace, > 0.f: Sphere Sweep
	 */
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraLibrary")
	static bool TraceSingleByObjectType(const UObject* WorldContextObject, FHitResult& OutHit, const FVector& Start,
		const FVector& End, const TArray<TEnumAsByte<ECollisionChannel>>& Channels, const TArray<AActor*>& ActorsToIgnore,
		float SweepRadius = 0.f, EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None, bool bTraceComplex = false);
	/** UKismetSystemLibrary::LineTrace / SphereTrace ForObjects
	 * @param Channels bTraceType HAS TO BE false for these channels
	 * @param SweepRadius == 0.f: Line Trace, > 0.f: Sphere Sweep
	 */
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraLibrary")
	static bool TraceMultiByObjectType(const UObject* WorldContextObject, TArray<FHitResult>& OutHits, const FVector& Start,
		const FVector& End, const TArray<TEnumAsByte<ECollisionChannel>>& Channels, const TArray<AActor*>& ActorsToIgnore,
		float SweepRadius = 0.f, EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None, bool bTraceComplex = false);

	/** @param SweepRadius == 0.f: Line Trace, > 0.f: Sphere Sweep */
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraLibrary")
	static bool TraceSingleByProfile(const UObject* WorldContextObject, FHitResult& OutHit, const FVector& Start,
		const FVector& End, const FCollisionProfileName& Profile, const TArray<AActor*>& ActorsToIgnore,
		float SweepRadius = 0.f, EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None, bool bTraceComplex = false);
	/** @param SweepRadius == 0.f: Line Trace, > 0.f: Sphere Sweep */
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraLibrary")
	static bool TraceMultiByProfile(const UObject* WorldContextObject, TArray<FHitResult>& OutHits, const FVector& Start,
		const FVector& End, const FCollisionProfileName& Profile, const TArray<AActor*>& ActorsToIgnore,
		float SweepRadius = 0.f, EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None, bool bTraceComplex = false);
#pragma endregion


	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraLibrary")
	static bool ConeOverlapLivingCharacters(const UObject* WorldContextObject, const FVector& Start, FVector Direction,
		float SlantHeight, float ConeHalfAngleDeg, const TArray<AActor*>& ActorsToIgnore,
		EDrawDebugTrace::Type DrawDebugType, TArray<AActor*>& OutCharacters);

	UFUNCTION(BlueprintCallable, Category="AuraLibrary")
	static void SortActorsByClosest(TArray<AActor*>& Actors, const FVector& Origin);

	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraLibrary|GameplayMechanics")
	static bool GetLiveCharactersInRadius(const UObject* WorldContextObject, TArray<AActor*>& OutActors,
		const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& Origin, bool bShowDebug = false);

	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject"), Category="AuraLibrary|GameplayMechanics")
	static bool IsAlly(const AActor* FirstActor, const AActor* SecondActor);
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject"), Category="AuraLibrary|GameplayMechanics")
	static bool IsNotAlly(const AActor* FirstActor, const AActor* SecondActor);

	UFUNCTION(BlueprintPure)
	static void FilterOutAllies(const AActor* InActor, TArray<AActor*>& ActorsToFilter); // Remove Allies from Array
	UFUNCTION(BlueprintPure)
	static void FilterOutEnemies(const AActor* InActor, TArray<AActor*>& ActorsToFilter); // Remove Enemies from Array
};
