// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "Interaction/ProjectileInterface.h"
#include "AuraProjectile.generated.h"

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
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn)) // Expose pin on spawn
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

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
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 2.f;
	
	// Flying sound
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAudioComponent> LoopingAudio;
	UPROPERTY(EditAnywhere, Category="Effects")
	TObjectPtr<USoundBase> ImpactSound;
	
	UPROPERTY(EditAnywhere, Category="Effects")
	TObjectPtr<UNiagaraSystem> ImpactEffect;
};
