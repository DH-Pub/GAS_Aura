// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "GameFramework/Actor.h"
#include "AuraProjectile.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Projectile_FireBolt_Impact)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Projectile_SlingShotRock_Impact)

/**
 *	The projectile AActor that will have VFX/Mesh/...
 */
UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
public:
	AAuraProjectile();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetHomingTarget(USceneComponent* Comp, const float AccelerationMagnitude = -1.f);

	UPROPERTY()
	TObjectPtr<class UProjectileAbility> SpawnedFromAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aura")
	float StartPitch = 0.f; // Pitch when projectile is spawned
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USphereComponent> Sphere;
private:
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	float LifeSpan = 2.f;

	UPROPERTY(EditDefaultsOnly, Category="Aura")
	float HomingTickRate = .2f;
	FTimerHandle HomingTimer;
	void HomingTick();

	UPROPERTY(EditDefaultsOnly, Category="Aura", meta=(GameplayTagFilter="GameplayCue.Projectile."))
	FGameplayTag GameplayCueImpact;
	UPROPERTY(EditAnywhere, Category="Aura")
	TObjectPtr<USoundBase> AttachedSound; // Sound that follows the projectile

	UPROPERTY()
	TObjectPtr<UAudioComponent> MovingSound; // Play while moving

	USceneComponent* AutoFindHomingTarget();
	UPROPERTY(EditAnywhere, Category="Aura|Homing")
	float FindHomingRadius = 0.f;
};
