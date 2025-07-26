// Copyright Hung


#include "AbilitySystem/Abilities/AuraProjectileAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// HasAuthority(&ActivationInfo);
}

void UAuraProjectileAbility::SpawnProjectile(const FVector& ProjectileTargetLocation, FGameplayTag SocketTag, const bool bStartFromCharacter,
	float SpawnDistance, float SpawnHeightAdd)
{
	if (!AvatarActor->HasAuthority()) return; // GetCurrentActivationInfo()
	if (!AvatarActor->Implements<UCombatInterface>()) return;
	FVector SpawnLocation = bStartFromCharacter ? AvatarActor->GetActorLocation() + AvatarActor->GetActorForwardVector() * SpawnDistance
		: ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor, SocketTag);

	// stop projectile from hitting the floor on spawned
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{ EObjectTypeQuery::ObjectTypeQuery1 };
	TArray<TObjectPtr<AActor>> ActorsToIgnore{AvatarActor};
	FHitResult FloorHitResult;
	UKismetSystemLibrary::LineTraceSingleForObjects(this, SpawnLocation, SpawnLocation - FVector(0.f, 0.f, 500.f),
		ObjectTypes, false, {AvatarActor}, EDrawDebugTrace::None, FloorHitResult, true);
	if (SpawnLocation.Z - FloorHitResult.ImpactPoint.Z < 50.f) SpawnLocation.Z = FloorHitResult.ImpactPoint.Z + SpawnHeightAdd;
	
	FRotator Rotation = (ProjectileTargetLocation - SpawnLocation).Rotation();
	Rotation.Pitch = 0.f;

	// SPAWNING
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform,
		AvatarActor, Cast<APawn>(AvatarActor), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	// const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AvatarActor);
	const UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AvatarActor);
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddSourceObject(Projectile);
	UAuraAbilitySystemLibrary::SetIsStaggerDamage(EffectContextHandle, bStagger);
	TArray<TWeakObjectPtr<AActor>> Actors;
	EffectContextHandle.AddActors(Actors);
	Projectile->DamageEffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	
	for (TPair<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
	{
		Projectile->DamageEffectSpecHandle.Data->SetSetByCallerMagnitude(Pair.Key, Pair.Value.GetValueAtLevel(GetAbilityLevel()));
	}

	Rotation.Pitch = Projectile->StartPitch;
	SpawnTransform.SetRotation(Rotation.Quaternion());
	Projectile->FinishSpawning(SpawnTransform);
}
