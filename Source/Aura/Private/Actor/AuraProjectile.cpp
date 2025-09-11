// Copyright Hung


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystem/AuraLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"

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

	LoopingAudio = CreateDefaultSubobject<UAudioComponent>("Audio");
	LoopingAudio->SetupAttachment(Sphere);
	LoopingAudio->bStopWhenOwnerDestroyed = true;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	LoopingAudio->Play();
}

//This won't be called on client if server destroy the object first
void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetInstigator()) return;
	if (!UAuraLibrary::IsNotFriend(GetInstigator(), OtherActor)) return;
	// Stop All projectile's functions, collisions
	ProjectileMovement->Deactivate();
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LoopingAudio->Stop();

	
	// Play Effect
	// UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	// UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	if (HasAuthority())
	{
		// Modify GameplayEffectSpecHandle (client does not have access)
		FGameplayEffectContextHandle GE_ContextHandle = DamageEffectSpecHandle.Data->GetContext();
		if (SweepResult.Distance == 0.f)
		{
			FHitResult* ZeroResult = const_cast<FHitResult*>(&SweepResult);
			ZeroResult->ImpactPoint = GetActorLocation();
			/*FHitResult HitResult;
			HitResult.Location = GE_ContextHandle.GetOrigin();
			HitResult.ImpactPoint = GetActorLocation();
			GE_ContextHandle.AddHitResult(HitResult);*/
		}
		GE_ContextHandle.AddHitResult(SweepResult);
		
		if (UAbilitySystemComponent* InstigatorASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetInstigator()))
		{
			FGameplayCueParameters CueParams;
			CueParams.EffectContext = GE_ContextHandle;
			CueParams.Location = GE_ContextHandle.GetHitResult()->ImpactPoint;
			CueParams.Instigator = GetInstigator();
			CueParams.EffectCauser = this;
			CueParams.SourceObject = OtherActor;
			InstigatorASC->ExecuteGameplayCue(AuraGameplayTags::GameplayCue_Impact_Projectile, CueParams);
		}
		
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data);
		}
		SetLifeSpan(2.f);
	}
}
