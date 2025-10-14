// Copyright Hung


#include "Actor/AuraProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"
#include "AuraAbilityLibrary.h"
#include "AuraEffectTypes.h"
#include "AbilitySystem/Ability/ProjectileAbility.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Aura/Aura.h"
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
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	if (AttachedSound)
	{
		/*UAudioComponent* MovingSound =*/ UGameplayStatics::SpawnSoundAttached(AttachedSound, GetRootComponent(), NAME_None,
			FVector(), GetActorRotation(), EAttachLocation::KeepRelativeOffset,
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


	if (!HasAuthority()) return; // Modify GameplayEffectSpecHandle (client does not have access)
	SetLifeSpan(.01f);
	if (SpawnedFromAbility == nullptr) return;
	const FGameplayEffectSpecHandle SpecHandle = SpawnedFromAbility->MakeDamageSpecHandle();
	FGameplayEffectContextHandle ContextHandle = SpecHandle.Data->GetContext();
	if (SweepResult.Distance < UE_SMALL_NUMBER)
	{	// const_cast<FHitResult&>(SweepResult).ImpactPoint = GetActorLocation();
		ContextHandle.AddOrigin(GetActorLocation());
	} else ContextHandle.AddOrigin(SweepResult.ImpactPoint);

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
	{	// if hit a character
		FDamageEffectContext& DamageContext = FAuraEffectContext::GetOrMakeContextStructRef<FDamageEffectContext>(ContextHandle.Get());
		DamageContext.DamageDirection = GetActorForwardVector();
		ContextHandle.AddSourceObject(this);
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}

	if (UAbilitySystemComponent* InstigatorASC = ContextHandle.GetInstigatorAbilitySystemComponent())
	{	// ExecuteGameplayCue if hit "Something"
		FGameplayCueParameters CueParams(ContextHandle);
		CueParams.Location = ContextHandle.GetOrigin();
		CueParams.Instigator = GetInstigator();
		CueParams.EffectCauser = this;
		CueParams.SourceObject = OtherActor;
		InstigatorASC->ExecuteGameplayCue(AuraGameplayTags::GameplayCue_Impact_Projectile, CueParams);
	}
}
