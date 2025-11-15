// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraProjectile.generated.h"

class UNiagaraSystem;

/**
 *	The projectile AActor that will have VFX/Mesh/...
 */
UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
public:
	AAuraProjectile();
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetHomingTarget(USceneComponent* Comp, const float AccelerationMagnitude = -1.f);
	
	UPROPERTY()
	TObjectPtr<class UProjectileAbility> SpawnedFromAbility;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Default")
	float StartPitch = 0.f; // Pitch when projectile is spawned
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USphereComponent> Sphere;
private:
	UPROPERTY(EditDefaultsOnly, Category="Default")
	float LifeSpan = 2.f;
	
	UPROPERTY(EditAnywhere, Category="Default")
	TObjectPtr<USoundBase> AttachedSound; // Sound that follows the projectile
	UPROPERTY(EditAnywhere, Category="Default")
	TObjectPtr<USoundBase> ImpactSound;
	UPROPERTY(EditAnywhere, Category="Default")
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> MovingSound; // Play while moving
	
	USceneComponent* AutoFindHomingTarget();
	UPROPERTY(EditAnywhere, Category="Default|Homing")
	float FindHomingRadius = 5000.f;
public:
	UFUNCTION(BlueprintCallable)
	static void ExecuteProjectileImpactCue(const struct FGameplayCueParameters& Params);
};
