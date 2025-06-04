// Copyright Hung


#include "AbilitySystem/Abilities/AuraProjectileAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// HasAuthority(&ActivationInfo);
}

void UAuraProjectileAbility::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor->HasAuthority()) return; // GetCurrentActivationInfo()

	if (const TScriptInterface<ICombatInterface> CombatInterface = AvatarActor)
	{
		const FVector SpawnLocation = AvatarActor->GetActorLocation() + AvatarActor->GetActorForwardVector() * 60.f;
		// const FVector SpawnLocation = CombatInterface->GetCombatSocketLocation();
		FRotator Rotation = (ProjectileTargetLocation - SpawnLocation).Rotation();
		Rotation.Pitch = 0.f;
		
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform,
			AvatarActor, Cast<APawn>(AvatarActor), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AvatarActor);
		
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
		
		Projectile->FinishSpawning(SpawnTransform);
	}
}
