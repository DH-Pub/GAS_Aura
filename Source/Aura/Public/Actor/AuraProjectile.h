// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ProjectileInterface.h"
#include "AuraProjectile.generated.h"

class UProjectileAbility;
class UNiagaraSystem;
class UProjectileMovementComponent;
class USphereComponent;

/**
 *	The projectile AActor that will have VFX/Mesh/...
 */
UCLASS()
class AURA_API AAuraProjectile : public AActor, public IProjectileInterface
{
	GENERATED_BODY()
public:
	AAuraProjectile();
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn))
	TObjectPtr<UProjectileAbility> SpawnedFromAbility;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	// Pitch when projectile is spawned
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Default")
	float StartPitch = 0.f;

	virtual void GetImpactCue_Implementation(UNiagaraSystem*& CueEffect, USoundBase*& CueSound) override
	{CueEffect = ImpactEffect; CueSound = ImpactSound;}
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;
private:
	UPROPERTY(EditDefaultsOnly, Category="Default")
	float LifeSpan = 2.f;

	UPROPERTY(EditAnywhere, Category="Default")
	TObjectPtr<USoundBase> AttachedSound; // Sound that follows the projectile
	UPROPERTY(EditAnywhere, Category="Default")
	TObjectPtr<USoundBase> ImpactSound;
	UPROPERTY(EditAnywhere, Category="Default")
	TObjectPtr<UNiagaraSystem> ImpactEffect;
};
