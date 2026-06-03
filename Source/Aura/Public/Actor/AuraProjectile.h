// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
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
	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastSetHomingTarget(USceneComponent* Comp, const float AccelerationMagnitude = -1.f);

	UPROPERTY()
	TObjectPtr<class UProjectileAbility> SpawnedFromAbility;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(ReplicatedUsing=OnRep_MaxTravelDistance, BlueprintReadWrite)
	float MaxTravelDistance = -1.f;
	UFUNCTION()
	void OnRep_MaxTravelDistance(float OldValue);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnMaxDistancePassed(); // By default, only Destroy() the projectile, override this
	FVector LastLocation;
	UPROPERTY(VisibleInstanceOnly, Category="Aura")
	float DistanceTraveled = 0.f;

	/**
	 * Allow piercing. <= 0: Infinite
	 * - In FPS, This can be a float called MaxPiercing: Armor/Wall with hardness float value and damage reduced after piercing
	 */
	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 MaxHitCount;
	UPROPERTY(VisibleInstanceOnly, Category="Aura")
	int32 CurrentHits = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura")
	float LifeSpan = 2.f; // -1 for infinite
protected:
	virtual void BeginPlay() override;
	virtual void OnHitActor(AActor* OtherActor, const FHitResult& SweepResult);
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	bool bPawnOnly = false; // Only Impact Pawn, else can hit walls, ...

	UPROPERTY(EditDefaultsOnly, Category="Aura")
	float ProjectileRadius = 0.f;
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TEnumAsByte<EDrawDebugTrace::Type> Debug;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnHitActor(const bool bIsAlly, AActor* OtherActor, const FHitResult& SweepResult);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Aura")
	TArray<AActor*> ActorsToIgnore;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Aura")
	TArray<AActor*> HitActors; // Actors That was Hit
private:
	UPROPERTY(EditDefaultsOnly, Category="Aura", meta=(GameplayTagFilter="GameplayCue.Projectile."))
	FGameplayTag GameplayCueImpact;
	UPROPERTY(EditAnywhere, Category="Aura")
	TObjectPtr<USoundBase> AttachedSound; // Sound that follows the projectile

	UPROPERTY()
	TWeakObjectPtr<UAudioComponent> MovingSound; // Play while moving

	USceneComponent* AutoFindHomingTarget();
	UPROPERTY(EditAnywhere, Category="Aura|Homing")
	float FindHomingRadius = 0.f;
	UPROPERTY(EditDefaultsOnly, Category="Aura|Homing")
	float HomingTickRate = .2f;
	FTimerHandle HomingTimer;
	void HomingTick();
};
