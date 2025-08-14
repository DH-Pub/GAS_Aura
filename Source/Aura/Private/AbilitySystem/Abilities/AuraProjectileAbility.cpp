// Copyright Hung


#include "AbilitySystem/Abilities/AuraProjectileAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "Character/AuraCharacterBase.h"
#include "Kismet/KismetSystemLibrary.h"

UAuraProjectileAbility::UAuraProjectileAbility()
{
}

void UAuraProjectileAbility::PreActivate(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	// HasAuthority(&ActivationInfo);
}

void UAuraProjectileAbility::SpawnProjectile(const FVector& TargetLocation, const FVector& InSpawnLocation,
	const bool bStartFromCharacter, const float SpawnDistance, const float SpawnHeightAdd)
{
	/*if (HasAuthorityOrPredictionKey(&GetActorInfo(), &GetCurrentActivationInfo()))
	HasAuthority(&GetCurrentActivationInfo());*/
	if (!AuraCharacterFromActorInfo->HasAuthority()) return; // GetCurrentActivationInfo()
	FVector Loc = bStartFromCharacter ? AuraCharacterFromActorInfo->GetActorLocation() + AuraCharacterFromActorInfo->GetActorForwardVector() * SpawnDistance
		: InSpawnLocation;

	// stop projectile from hitting the floor on spawned
	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{ EObjectTypeQuery::ObjectTypeQuery1 };
	FHitResult FloorHitResult;
	UKismetSystemLibrary::LineTraceSingleForObjects(this, Loc, Loc - FVector(0.f, 0.f, 200.f),
		ObjectTypes, false, {AuraCharacterFromActorInfo}, EDrawDebugTrace::None, FloorHitResult, true);
	if (Loc.Z - FloorHitResult.ImpactPoint.Z < 50.f) Loc.Z = FloorHitResult.ImpactPoint.Z + SpawnHeightAdd;
	
	FRotator Rotation = (TargetLocation - Loc).Rotation();
	Rotation.Pitch = 0.f;

	// SPAWNING
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(Loc);
	SpawnTransform.SetRotation(Rotation.Quaternion());
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform,
		AuraCharacterFromActorInfo, AuraCharacterFromActorInfo, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	// const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AuraCharacter);
	const UAuraAbilitySystemComponent* SourceASC = AuraCharacterFromActorInfo->GetAuraAbilitySystemComponent();

	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddSourceObject(Projectile);
	TArray<TWeakObjectPtr<AActor>> Actors;
	EffectContextHandle.AddActors(Actors);
	UAuraAbilitySystemLibrary::SetIsStaggerDamage(EffectContextHandle, bStagger);
	
	Projectile->DamageEffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	for (TPair<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
	{
		Projectile->DamageEffectSpecHandle.Data->SetSetByCallerMagnitude(Pair.Key, Pair.Value.GetValueAtLevel(GetAbilityLevel()));
	}

	Projectile->FinishSpawning(SpawnTransform);
}
