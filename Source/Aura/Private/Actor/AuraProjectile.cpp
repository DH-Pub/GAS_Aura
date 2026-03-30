// Copyright Hung


#include "Actor/AuraProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilityLibrary.h"
#include "AuraTag.h"
#include "AbilitySystem/Ability/DamageAbility.h"
#include "AbilitySystem/Ability/ProjectileAbility.h"
#include "Character/AuraCharacterBase.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Projectile_FireBolt_Impact, "GameplayCue.Projectile.FireBolt.Impact")
UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Projectile_SlingShotRock_Impact, "GameplayCue.Projectile.SlingShotRock.Impact")

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; // for the projectile to spawn for all
	// SetReplicatingMovement(true);

	Sphere = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SetRootComponent(Sphere);
	Sphere->bReturnMaterialOnMove = true; // for FHitResult.PhysMaterial to be non-null

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovement->InitialSpeed = 800.f;
	ProjectileMovement->MaxSpeed = 800.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	SetNetUpdateFrequency(40);
}

void AAuraProjectile::MulticastSetHomingTarget_Implementation(USceneComponent* Comp, const float AccelerationMagnitude)
{
	if (Comp == nullptr)
	{
		AutoFindHomingTarget();
	}
	if (Comp == nullptr)
	{
		ProjectileMovement->HomingTargetComponent = nullptr;
		ProjectileMovement->ProjectileGravityScale = 1.f;
		return;
	}
	AAuraCharacterBase* AuraCharacterBase = Cast<AAuraCharacterBase>(Comp->GetOwner());
	if (AuraCharacterBase == nullptr || ICombatInterface::Execute_IsDead(AuraCharacterBase)) return;

	ProjectileMovement->HomingTargetComponent = Comp;
	if (AccelerationMagnitude > UE_KINDA_SMALL_NUMBER)
	{
		ProjectileMovement->HomingAccelerationMagnitude = AccelerationMagnitude;
	}
	ProjectileMovement->ProjectileGravityScale = 0.1f;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	if (ProjectileMovement->bIsHomingProjectile)
	{
		GetWorld()->GetTimerManager().SetTimer(HomingTimer, this, &AAuraProjectile::HomingTick,
			HomingTickRate, true);
	}
	if (AttachedSound)
	{
		MovingSound = UGameplayStatics::SpawnSoundAttached(AttachedSound, GetRootComponent(),
			NAME_None, FVector(), GetActorRotation(), EAttachLocation::KeepRelativeOffset,
			true, 1.f, 1.f, 0.f);
	}
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TArray<AActor*> ActorsToIgnore;
	UAuraAbilityLibrary::AddAdditionalTraceIgnoreActors(ActorsToIgnore, this);
	if (ActorsToIgnore.Contains(OtherActor)) return;
	if (UAuraAbilityLibrary::IsAlly(GetInstigator(), OtherActor)) return;
	FVector Velocity = GetVelocity(); // ->Deactivate() will make this 0
	FVector VelocityNormal = Velocity.GetSafeNormal();

	// Stop All projectile's functions, collisions
	ProjectileMovement->Deactivate();
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (MovingSound) MovingSound->Stop();
	GetWorld()->GetTimerManager().ClearTimer(HomingTimer);

	AAuraCharacterBase* Character = Cast<AAuraCharacterBase>(OtherActor);
	const FVector Loc = GetActorLocation();
	FHitResult& ConstCastResult = const_cast<FHitResult&>(SweepResult);
	if (Character)
	{
		TArray<FHitResult> HitResults;
		UAuraAbilityLibrary::TraceByChannel(this, Loc - VelocityNormal * 10.f,
			Loc + VelocityNormal * 10.f, ActorsToIgnore, EDrawDebugTrace::None, HitResults,
			{ECC_AuraTrace_Effect}, Sphere->GetScaledSphereRadius() + 1.f);
		for (FHitResult& Hit : HitResults)
		{
			if (Hit.GetActor() != OtherActor) continue;
			ConstCastResult = MoveTemp(Hit);
			break;
		}
	}
	ConstCastResult.bBlockingHit = true;
	if (SweepResult.bStartPenetrating)
	{
		ConstCastResult.ImpactPoint = GetActorLocation();
		if (Character && Sphere->GetScaledSphereRadius() > Character->GetCapsuleComponent()->GetScaledCapsuleRadius())
		{	// if (Projectile is bigger than the character
			ConstCastResult.ImpactPoint = Character->GetActorLocation();
		}
	}
	if (UAbilitySystemComponent* InstigatorASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetInstigator()))
	{	/* UGameplayStatics::PlaySoundAtLocation(); UNiagaraFunctionLibrary::SpawnSystemAtLocation(); */
		FGameplayCueParameters Params;
		Params.Location = SweepResult.ImpactPoint;// ProjectileLoc;
		Params.Normal = SweepResult.ImpactNormal;
		Params.PhysicalMaterial = SweepResult.PhysMaterial;// ::MakeGameplayCueParametersFromHitResult
		Params.Instigator = GetInstigator(); Params.EffectCauser = this;
		InstigatorASC->InvokeGameplayCueEvent(GameplayCueImpact, EGameplayCueEvent::Executed, Params);
	}

	if (HasAuthority())
	{
		SetLifeSpan(.15f); // Function won't be called on client if server destroy the object first
		if (!SpawnedFromAbility) return;
		if (!Character) return;

		TArray<FGameplayEffectSpecHandle> SpecHandles = SpawnedFromAbility->MakeOutgoingAbilityEffectsSpecs();
		for (const FGameplayEffectSpecHandle& SpecHandle : SpecHandles)
		{
			FGameplayEffectContext* Context = SpecHandle.Data->GetContext().Get();
			Context->AddOrigin(SweepResult.ImpactPoint);
			Context->SetEffectCauser(this);
			Context->AddHitResult(SweepResult);
		}
		Velocity.Z = 0.f;
		SpawnedFromAbility->ApplyAbilityEffectsToTarget(OtherActor, SpecHandles, Velocity);
	}
}

void AAuraProjectile::HomingTick()
{
	if (USceneComponent* HomingComp = ProjectileMovement->HomingTargetComponent.Get())
	{
		if (AAuraCharacterBase* Chara = Cast<AAuraCharacterBase>(HomingComp->GetOwner()))
		{
			if (ICombatInterface::Execute_IsDead(Chara))
			{
				if (HasAuthority()) MulticastSetHomingTarget(AutoFindHomingTarget());
			}
		}
	}
}

USceneComponent* AAuraProjectile::AutoFindHomingTarget()
{
	const FVector CurrentLocation = GetActorLocation();
	TArray<AActor*> ActorsToIgnore = {this, GetInstigator(), GetOwner(),};
	if (ProjectileMovement->HomingTargetComponent.Get())
	{
		ActorsToIgnore.Add(ProjectileMovement->HomingTargetComponent->GetOwner());
	}
	FCollisionQueryParams SphereParams; SphereParams.AddIgnoredActors(ActorsToIgnore);
	TArray<FOverlapResult> Overlaps; // UKismetSystemLibrary::SphereOverlapActors();
	if (GetWorld() == nullptr) return nullptr;
	GetWorld()->OverlapMultiByObjectType(Overlaps, CurrentLocation, FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(FindHomingRadius), SphereParams);
	TArray<AActor*> Actors;
	for (FOverlapResult& Overlap : Overlaps)
	{
		AActor* OverlapActor = Overlap.GetActor();
		if (OverlapActor->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(OverlapActor) &&
			UAuraAbilityLibrary::IsNotAlly(OverlapActor, GetInstigator()))
		{
			Actors.AddUnique(OverlapActor);
		}
	}
	float NearestDist = 0.f;
	if (const AActor* NearestActor = UGameplayStatics::FindNearestActor(CurrentLocation, Actors, NearestDist))
	{
		return NearestActor->GetRootComponent();
	}
	return nullptr;
}
