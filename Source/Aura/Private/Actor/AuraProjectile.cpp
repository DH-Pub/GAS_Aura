// Copyright Hung


#include "Actor/AuraProjectile.h"

#include "AbilitySystemGlobals.h"
#include "AuraAbilityLibrary.h"
#include "AuraTag.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/Ability/DamageAbility.h"
#include "AbilitySystem/Ability/ProjectileAbility.h"
#include "Character/AuraCharacterBase.h"
#include "Components/AudioComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Projectile_FireBolt_Impact, "GameplayCue.Projectile.FireBolt.Impact")
UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Projectile_SlingShotRock_Impact, "GameplayCue.Projectile.SlingShotRock.Impact")

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true; // for the projectile to spawn for all
	// SetReplicatingMovement(true);

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root"));

	/*Sphere = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SetRootComponent(Sphere);
	Sphere->bReturnMaterialOnMove = true; // for FHitResult.PhysMaterial to be non-null*/

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovement->InitialSpeed = 800.f;
	ProjectileMovement->MaxSpeed = 800.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	SetNetUpdateFrequency(40);
}

void AAuraProjectile::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	const FVector CurrentLoc = GetActorLocation();

	static TArray<FHitResult> Results; Results.Reset();
	static TArray<AActor*> Ignores; Ignores.Reset();
	Ignores.Append(ActorsToIgnore);
	Ignores.Append(HitActors); // Disable Multiple Hit on Piercing because of Ticking while inside actor
	if (bPawnOnly)
	{
		UAuraAbilityLibrary::TraceMultiByObjectType(this, Results, LastLocation, CurrentLoc,
			{ECC_Pawn}, Ignores, ProjectileRadius, Debug);
	}
	else
	{
		UAuraAbilityLibrary::TraceMultiByChannel(this, Results, LastLocation, CurrentLoc,
			ECC_AuraTrace_EffectMulti, Ignores, ProjectileRadius, Debug);
	}

	for (FHitResult& Hit : Results)
	{
		OnHitActor(Hit.GetActor(), Hit);
	}

	if (MaxTravelDistance > 0.f)
	{
		DistanceTraveled += (CurrentLoc - LastLocation).Size();
		if (DistanceTraveled > MaxTravelDistance)
		{
			if (HasAuthority())
			{
				const float OldMaxTravelDistance = MaxTravelDistance;
				/* Clear so we will not go inside this block and call OnMaxDistancePassed() again
				 * because projectile can stay over MaxTravelDistance more than 1 tick */
				MaxTravelDistance = -1.f;
				OnRep_MaxTravelDistance(OldMaxTravelDistance);
			}
		}
	}

	LastLocation = CurrentLoc;
}

void AAuraProjectile::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AAuraProjectile, MaxTravelDistance, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(AAuraProjectile, MaxHitCount, COND_None, REPNOTIFY_OnChanged);
}

void AAuraProjectile::OnRep_MaxTravelDistance(const float OldValue)
{
	if (OldValue > 0.f && MaxTravelDistance < 0.f)
	{
		OnMaxDistancePassed();
	}
}

void AAuraProjectile::OnMaxDistancePassed_Implementation()
{
	Destroy();
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
	LastLocation = GetActorLocation();
	UAuraAbilityLibrary::AddAdditionalTraceIgnoreActors(ActorsToIgnore, this);

	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
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

void AAuraProjectile::OnHitActor(AActor* OtherActor, const FHitResult& SweepResult)
{
	const bool bAlly = UAuraAbilityLibrary::IsAlly(GetInstigator(), OtherActor);
	BP_OnHitActor(bAlly, OtherActor, SweepResult);
	if (bAlly) return;
	FVector Velocity = GetVelocity(); // ->Deactivate() will make this 0
	const bool bHasAuth = HasAuthority();

	if (MaxHitCount > 0)
	{
		if (++CurrentHits >= MaxHitCount)
		{	// Stop All projectile's functions, collisions
			ProjectileMovement->Deactivate();
			SetActorTickEnabled(false);
			SetActorHiddenInGame(true);
			if (MovingSound.IsValid()) MovingSound->Stop();
			GetWorld()->GetTimerManager().ClearTimer(HomingTimer);

			if (bHasAuth)
			{
				SetLifeSpan(.15f); // Function won't trigger on Client if this is destroyed first on Server
			}
		}
	}

	if (GameplayCueImpact.IsValid())
	{	// Actor doesn't have ASC so the event will only be local. UGameplayCueFunctionLibrary::ExecuteGameplayCueOnActor()
		FGameplayCueParameters Params;// ::MakeGameplayCueParametersFromHitResult
		Params.Location = SweepResult.ImpactPoint; Params.Normal = SweepResult.ImpactNormal;
		Params.PhysicalMaterial = SweepResult.PhysMaterial;
		Params.Instigator = GetInstigator(); Params.EffectCauser = this;
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(this, GameplayCueImpact, Params);
	} /*UGameplayStatics::PlaySoundAtLocation(); UNiagaraFunctionLibrary::SpawnSystemAtLocation();*/

	if (bHasAuth && SpawnedFromAbility && UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
	{
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

	HitActors.Add(OtherActor);  // Disable Multiple Hit on Piercing because of Ticking while inside actor
}

USceneComponent* AAuraProjectile::AutoFindHomingTarget()
{
	const FVector CurrentLocation = GetActorLocation();
	FCollisionQueryParams SphereParams; SphereParams.AddIgnoredActors(ActorsToIgnore);
	if (ProjectileMovement->HomingTargetComponent.Get())
	{
		SphereParams.AddIgnoredActor(ProjectileMovement->HomingTargetComponent->GetOwner());
	}
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

void AAuraProjectile::HomingTick()
{
	if (const USceneComponent* HomingComp = ProjectileMovement->HomingTargetComponent.Get())
	{
		const AAuraCharacterBase* Chara = Cast<AAuraCharacterBase>(HomingComp->GetOwner());
		if (Chara && ICombatInterface::Execute_IsDead(Chara))
		{
			if (HasAuthority()) MulticastSetHomingTarget(AutoFindHomingTarget());
		}
	}
}
