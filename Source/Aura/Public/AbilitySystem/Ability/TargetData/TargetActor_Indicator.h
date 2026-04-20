// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "AbilitySystem/Ability/BeamAbility.h"
#include "TargetActor_Indicator.generated.h"

USTRUCT(BlueprintType)
struct FGATargetData_CommonTarget : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	FGATargetData_CommonTarget(){}

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> TargetActor;
	/** Returns all actors targeted, almost always overridden */
	virtual TArray<TWeakObjectPtr<AActor>> GetActors() const override {return {TargetActor};}

	UPROPERTY(BlueprintReadWrite)
	FVector_NetQuantizeNormal Direction;
	UPROPERTY(BlueprintReadWrite)
	FVector_NetQuantize10 Location;
	virtual FVector GetEndPoint() const override {return Location;}

	// Required for all child structs of FGameplayAbilityTargetData
	virtual UScriptStruct* GetScriptStruct() const override {return StaticStruct();}
	// Required for all child structs of FGameplayAbilityTargetData
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << TargetActor;
		Direction.NetSerialize(Ar, Map, bOutSuccess);
		Location.NetSerialize(Ar, Map, bOutSuccess);
		return bOutSuccess;
	}
};
template<>
struct TStructOpsTypeTraits<FGATargetData_CommonTarget> : public TStructOpsTypeTraitsBase2<FGATargetData_CommonTarget>
{	// This is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	enum {WithNetSerializer = true};
};

USTRUCT(BlueprintType, Blueprintable)
struct FIndicatorDetails
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USceneComponent> OriginComponent;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName SocketName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxRange = 500.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCollisionProfileName TraceProfile; // ground trace to place Indicator
};


DECLARE_MULTICAST_DELEGATE_OneParam(FOnServerSpawned, class ATargetActor_Indicator*);


UCLASS()
class AURA_API ATargetActor_Indicator : public AActor
{
	GENERATED_BODY()
public:
	ATargetActor_Indicator();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Unreliable)
	void ServerSetLocation(FVector_NetQuantize10 InLoc, FVector_NetQuantizeNormal Direction = FVector::DownVector);
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSetLocation(FVector_NetQuantize10 InLoc, FVector_NetQuantizeNormal Direction = FVector::DownVector);
	void CheckAndSetLocation(FVector InLoc, FVector Direction = FVector::DownVector);

	UFUNCTION(BlueprintNativeEvent)
	FGameplayAbilityTargetDataHandle GetIndicatorDataHandle();

	UPROPERTY(Replicated, BlueprintReadWrite, EditInstanceOnly, meta=(ExposeOnSpawn), Category="Aura")
	FIndicatorDetails Details;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, config, meta=(ExposeOnSpawn), Category="Aura")
	TEnumAsByte<EDrawDebugTrace::Type> Debug;

	FOnServerSpawned OnServerSpawned;

	FGameplayAbilitySpecHandle AbilityHandle;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UDecalComponent> Decal;
};
