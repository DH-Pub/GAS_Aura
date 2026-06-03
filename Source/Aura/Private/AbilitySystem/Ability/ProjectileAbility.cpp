// Copyright Hung


#include "AbilitySystem/Ability/ProjectileAbility.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityLibrary.h"
#include "AbilitySystem/AbilityTask/AT_WaitData.h"
#include "Actor/AuraProjectile.h"
#include "Character/AuraCharacterBase.h"

void UProjectileAbility::SpawnProjectile(FVector& SpawnLoc, FRotator& InRot, const AActor* HomingTarget, float HeightAdd)
{
	// stop projectile from hitting the floor on spawned
	if (FHitResult FloorHitResult; UAuraAbilityLibrary::TraceSingleByObjectType(this, FloorHitResult, SpawnLoc,
		SpawnLoc + FVector::DownVector * HeightAdd, {ECC_WorldDynamic}, {}))
	{
		SpawnLoc.Z = FloorHitResult.ImpactPoint.Z + HeightAdd;
	}

	InRot.Pitch = ProjectilePitch;
	FTransform SpawnTransform(SpawnLoc);

	const int32 NumProjectiles = ProjectileNums.GetValueAtLevel(GetAbilityLevel());
	const float DeltaSpread = ProjectileSpread / NumProjectiles;
	FRotator RotatingRot = InRot - FRotator(0.f, ProjectileSpread / 2.f, 0.f);

	const bool bHasAuth = GetCurrentActorInfo()->IsNetAuthority();
	const bool bPredictingClient = IsPredictingClient();
	const bool bHomingValid = bHoming && HomingTarget && HomingTarget->Implements<UCombatInterface>();

	for (int32 i = 0; i < NumProjectiles; i++)
	{
		FRotator Rot(RotatingRot.Pitch, RotatingRot.Yaw + DeltaSpread * (i + .5f), RotatingRot.Roll);
		SpawnTransform.SetRotation(Rot.Quaternion());

		if (bHasAuth)
		{	// Spawn on server
			AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass,
				SpawnTransform, nullptr /*Owned by the staff, but it's not an actor*/, AuraCharacter,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn /*can spawn inside others*/);
			Projectile->SpawnedFromAbility = this;
			Projectile->MaxTravelDistance = MaxTravelDistance;
			Projectile->MaxHitCount = MaxHitCount;

			Projectile->FinishSpawning(SpawnTransform);

			ProjectilesSpawned.Add(Projectile);
			Projectile->OnDestroyed.AddDynamic(this, &UProjectileAbility::OnProjectileDestroyed);

			if (bHomingValid)
			{
				Projectile->MulticastSetHomingTarget(HomingTarget->GetRootComponent(), HomingAcceleration);
			}
		}
		else if (bPredictingClient)
		{
			bool bSpawn = false;
		}
	}
}

void UProjectileAbility::OnProjectileDestroyed(AActor* DestroyedActor)
{
	AAuraProjectile* DestroyedProjectile = Cast<AAuraProjectile>(DestroyedActor);
	ProjectilesSpawned.Remove(DestroyedProjectile);
	BP_OnProjectileDestroyed(DestroyedProjectile);
	// ProjectilesSpawned.RemoveAll(nullptr);
}

void UProjectileAbility::GetAbilityDetails(FAbilityDetails& Details) const
{
	Super::GetAbilityDetails(Details);
	Details.ProjectileNums = ProjectileNums.GetValueAtLevel(Details.Level);
}


// AbilityTask ======================================================================================================
UAbilityTask_SpawnProjectile* UAbilityTask_SpawnProjectile::SpawnProjectile(UProjectileAbility* OwningAbility,
	const ECombatSocket SpawnSocket, const FVector& Direction, AActor* Target, float HeightIfHitGround)
{
	UAbilityTask_SpawnProjectile* Task = NewAbilityTask<UAbilityTask_SpawnProjectile>(OwningAbility);
	Task->ProjectileAbility = OwningAbility;
	Task->AuraCharacter = Task->ProjectileAbility->AuraCharacter;
	Task->Location = Task->AuraCharacter->GetCombatSocketLocation(SpawnSocket);
	Task->Direction = Direction;
	Task->Target = Target;
	Task->HeightIfHitGround = HeightIfHitGround;
	return Task;
}

void UAbilityTask_SpawnProjectile::Activate()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC) return;
	if (IsLocallyControlled())
	{
		// Player input SHOULD be instantly predicted (e.g. 'hold down and charge')
		FScopedPredictionWindow(ASC, IsPredictingClient()); // call Destroy finish when out of scope
		if (IsPredictingClient())
		{
			FGATargetData_CommonData* Data = new FGATargetData_CommonData();
			Data->Location = Location;
			Data->Direction = Direction;
			// Data->ActivationTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
			FGameplayAbilityTargetDataHandle DataHandle(Data);

			ASC->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(),
				GetActivationPredictionKey(), DataHandle, FGameplayTag(),
				ASC->ScopedPredictionKey);
		}
		else
		{	// Server
			FRotator Rotation = Direction.ToOrientationRotator();
			ProjectileAbility->SpawnProjectile(Location, Rotation, Target, HeightIfHitGround);
		}
		if (ShouldBroadcastAbilityTaskDelegates()) OnSpawnFinish.Broadcast();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

		DelegateHandle = ASC->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(
			this, &UAbilityTask_SpawnProjectile::OnTargetDataReplicatedCallback);
		if (!ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey))
		{
			SetWaitingOnRemotePlayerData(); // if data hasn't reached the server yet
		}
	}
}

void UAbilityTask_SpawnProjectile::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		const FGameplayAbilitySpecHandle Spec = GetAbilitySpecHandle();
		const FPredictionKey PredictionKey = GetActivationPredictionKey();
		ASC->AbilityTargetDataSetDelegate(Spec, PredictionKey).Remove(DelegateHandle);
		ASC->ConsumeClientReplicatedTargetData(Spec, PredictionKey);
	}

	if (const FGameplayAbilityTargetData* Data = DataHandle.Get(0))
	{
		const FTransform ClientTransformData = Data->GetOrigin();
		const FVector ClientSpawnLocation = ClientTransformData.GetTranslation();
		FVector Difference = ClientSpawnLocation - Location;
		const float DifferenceSizeSqr = Difference.SizeSquared();
		if (DifferenceSizeSqr > 0.f)
		{
			constexpr float Tolerance = 100.f;
			if (DifferenceSizeSqr < Tolerance * Tolerance) Location = ClientSpawnLocation;
			else if (Difference.Normalize()) Location += Difference * Tolerance;
		}
		// const FVector Floats = ClientTransformData.GetScale3D(); // Arbitrary numbers are stored in Scale
		// const float TimeDifferent = GetWorld()->GetGameState()->GetServerWorldTimeSeconds() - Floats.X;

		FRotator Rotation = ClientTransformData.Rotator();
		ProjectileAbility->SpawnProjectile(Location, Rotation, Target, HeightIfHitGround);

		if (ShouldBroadcastAbilityTaskDelegates()) OnSpawnFinish.Broadcast();
	}
}
