// Copyright Hung


#include "AbilitySystem/Ability/ProjectileAbility.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Character/AuraCharacterBase.h"

void UProjectileAbility::SpawnProjectile(const FVector& InSpawnLocation, AActor* MovingTarget, bool bStartFromCharacter,
                                         const float SpawnDistance, float SpawnHeightAdd, float Pitch)
{
	if (!HasAuthorityOrPredictionKey(GetCurrentActorInfo(), &GetCurrentActivationInfoRef())) return;
	FVector SpawnLoc = bStartFromCharacter ?
		AuraCharacter->GetActorLocation() + AuraCharacter->GetActorForwardVector() * SpawnDistance
		: InSpawnLocation;

	// stop projectile from hitting the floor on spawned
	FHitResult FloorHitResult; FCollisionObjectQueryParams Params(ECC_WorldStatic);
	GetWorld()->LineTraceSingleByObjectType(FloorHitResult, SpawnLoc, SpawnLoc + FVector(0.f, 0.f, -200.f), Params);
	if (SpawnLoc.Z - FloorHitResult.ImpactPoint.Z < 50.f) SpawnLoc.Z = FloorHitResult.ImpactPoint.Z + SpawnHeightAdd;

	FRotator Rotation = AuraCharacter->AimDirection.Rotation();
	Rotation.Pitch = Pitch;
	FTransform SpawnTransform(Rotation); // SpawnTransform.SetRotation(Rotation.Quaternion());
	SpawnTransform.SetLocation(SpawnLoc);

	const int32 NumProjectiles = ProjectileNums.GetValueAtLevel(GetAbilityLevel());
	const float DeltaSpread = ProjectileSpread / NumProjectiles;
	FRotator LeftMostRot = Rotation - FRotator(0.f, ProjectileSpread / 2.f, 0.f);
	for (int32 i = 0; i < NumProjectiles; i++)
	{
		FRotator Rot = FRotator(LeftMostRot.Pitch, LeftMostRot.Yaw + DeltaSpread * (i + 0.5), LeftMostRot.Roll);
		SpawnTransform.SetRotation(Rot.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform,
			nullptr /*Owned by the staff, but it's not an actor*/, AuraCharacter,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn /*can spawn inside other actor, HitResult.Distance == 0.f*/);
		Projectile->SpawnedFromAbility = this;

		Projectile->FinishSpawning(SpawnTransform);
	}
}
