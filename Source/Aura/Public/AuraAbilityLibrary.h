// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilityLibrary.generated.h"

namespace EDrawDebugTrace {enum Type : int;}
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

	/** UKismetSystemLibrary::LineTrace SphereTrace ForObjects
	 * @param Channels The channels used for trace or to trace for if is object type
	 * @param bTraceType true: Trace Channel, else Trace for Objects
	 * @param SweepRadius == 0.f: Line Trace, > 0.f: Sphere Sweep
	 * @return OutHits.Num() > 0
	 */
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraLibrary")
	static bool TraceByChannel(const UObject* WorldContextObject, const FVector& Start, const FVector& End,
		const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits,
		const TArray<TEnumAsByte<ECollisionChannel>>& Channels, const bool bTraceType = true, const float SweepRadius = 0.f,
		const bool bTraceComplex = false);

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
