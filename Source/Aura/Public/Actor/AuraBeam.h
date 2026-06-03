// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "AuraBeam.generated.h"

USTRUCT()
struct FGATargetData_BeamData : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	/** We could be selecting this group of actors from any type of location, so use a generic location type */
	UPROPERTY()
	TArray<FVector_NetQuantize> SourceLocation;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> TargetActorArray;
	virtual TArray<TWeakObjectPtr<AActor> >	GetActors() const override {return TargetActorArray;}

	// Required for all child structs of FGameplayAbilityTargetData
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct();}
	// Required for all child structs of FGameplayAbilityTargetData
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
	{
		bOutSuccess |= SafeNetSerializeTArray_WithNetSerialize<31>(Ar, SourceLocation, Map);
		bOutSuccess |= SafeNetSerializeTArray_Default<31>(Ar, TargetActorArray);
		return bOutSuccess;
	}
};
template<>
struct TStructOpsTypeTraits<FGATargetData_BeamData> : public TStructOpsTypeTraitsBase2<FGATargetData_BeamData>
{
	enum
	{
		WithNetSerializer = true // For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};

UCLASS()
class AURA_API AAuraBeam : public AActor
{
	GENERATED_BODY()
public:
	AAuraBeam();

	virtual void Destroyed() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetTarget(FGATargetData_BeamData Data);


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UNiagaraComponent> BeamComponent;
	UPROPERTY()
	TArray<TObjectPtr<UNiagaraComponent>> ChainBeamsNiagara;

	UPROPERTY(EditDefaultsOnly, Category="Aura")
	FFXSystemSpawnParameters ChainSpawnParams;

	FGATargetData_BeamData BeamData;
	void SetBeamByData();
public:
	virtual void Tick(float DeltaTime) override;
};
