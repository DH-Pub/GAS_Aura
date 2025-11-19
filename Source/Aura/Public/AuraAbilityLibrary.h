// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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
	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"), Category="AuraLibrary|Actor")
	static bool YawActorToRotation(AActor* InActor, FVector InAimDirection,
		float DeltaTime, float InterpSpeed); // return true if finished

	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraLibrary|GameplayMechanics")
	static void GetLivePlayersInRadius(const UObject* WorldContextObject, TArray<AActor*>& OutActors,
		const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& Origin, bool bShowDebug = false);
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject"), Category="AuraLibrary|GameplayMechanics")
	static bool IsNotFriend(const AActor* FirstActor, const AActor* SecondActor);
};
