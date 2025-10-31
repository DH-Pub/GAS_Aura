// Copyright Hung


#include "Actor/AuraProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilityLibrary.h"
#include "AuraEffectTypes.h"
#include "AuraGameplayTags.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/Ability/DamageAbility.h"
#include "AbilitySystem/Ability/ProjectileAbility.h"
#include "Aura/Aura.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; // for the projectile to spawn for all

	Sphere = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovement->InitialSpeed = 800.f;
	ProjectileMovement->MaxSpeed = 800.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	SetNetUpdateFrequency(40);
}

void AAuraProjectile::MulticastSetHomingTarget_Implementation(USceneComponent* Comp, const float AccelerationMagnitude)
{
	ProjectileMovement->ProjectileGravityScale = 0.1f;
	ProjectileMovement->bIsHomingProjectile = true;
	ProjectileMovement->HomingTargetComponent = Comp;
	ProjectileMovement->HomingAccelerationMagnitude = AccelerationMagnitude;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	if (AttachedSound)
	{
		/*UAudioComponent* MovingSound =*/ UGameplayStatics::SpawnSoundAttached(AttachedSound, GetRootComponent(),
			NAME_None, FVector(), GetActorRotation(), EAttachLocation::KeepRelativeOffset,
			true, 1.f, 1.f, 0.f);
	}
}

//This won't be called on client if server destroy the object first
void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetInstigator()) return;
	if (!UAuraAbilityLibrary::IsNotFriend(GetInstigator(), OtherActor)) return;
	// Stop All projectile's functions, collisions
	ProjectileMovement->Deactivate();
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/*const FVector Loc = GetActorLocation();
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Loc);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, Loc);*/

	if (UAbilitySystemComponent* InstigatorASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetInstigator()))
	{
		FGameplayCueParameters Params;
		Params.EffectCauser = this;
		Params.Location = GetActorLocation();
		/** Handles gameplay cue locally */
		InstigatorASC->InvokeGameplayCueEvent(AuraGameplayTags::GameplayCue_Impact_Projectile,
			EGameplayCueEvent::Executed, Params);
	}
	if (!HasAuthority()) return; // Modify GameplayEffectSpecHandle (client does not have access)
	SetLifeSpan(.1f);
	if (SpawnedFromAbility == nullptr) return;
	const FGameplayEffectSpecHandle SpecHandle = SpawnedFromAbility->MakeDamageSpecHandle();
	FGameplayEffectContextHandle ContextHandle = SpecHandle.Data->GetContext();
	ContextHandle.Get()->SetEffectCauser(this);
	if (SweepResult.Distance < UE_SMALL_NUMBER)
	{	// const_cast<FHitResult&>(SweepResult).ImpactPoint = GetActorLocation();
		ContextHandle.AddOrigin(GetActorLocation());
	} else ContextHandle.AddOrigin(SweepResult.ImpactPoint);

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
	{	// if hit a character
		FDamageEffectContext* DamageContext = FAuraEffectContext::MakeStructInContext<FDamageEffectContext>(ContextHandle);
		DamageContext->DamageDirection = GetActorForwardVector();
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
}

void AAuraProjectile::ExecuteProjectileImpactCue(const FGameplayCueParameters& Params)
{
	if (const AAuraProjectile* Projectile = Cast<AAuraProjectile>(Params.GetEffectCauser()))
	{
		UGameplayStatics::PlaySoundAtLocation(Projectile, Projectile->ImpactSound, Params.Location);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(Projectile, Projectile->ImpactEffect, Params.Location);
	}
}
