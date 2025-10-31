// Copyright Hung


#include "AbilitySystem/Ability/ProjectileAbility.h"

#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Character/AuraCharacterBase.h"
#include "GameFramework/GameStateBase.h"

void UProjectileAbility::SpawnProjectile(FVector& SpawnLoc, const FVector& CursorHit, FRotator InRot, const AActor* HomingTarget,
	float HeightAdd)
{
	// stop projectile from hitting the floor on spawned
	FHitResult FloorHitResult; FCollisionObjectQueryParams Params(ECC_WorldStatic);
	GetWorld()->LineTraceSingleByObjectType(FloorHitResult, SpawnLoc,
		SpawnLoc + FVector(0.f, 0.f, -200.f), Params);
	if (SpawnLoc.Z - FloorHitResult.ImpactPoint.Z < 50.f) SpawnLoc.Z = FloorHitResult.ImpactPoint.Z + HeightAdd;

	InRot.Pitch = ProjectilePitch;
	FTransform SpawnTransform(InRot); // SpawnTransform.SetRotation(Rotation.Quaternion());
	SpawnTransform.SetLocation(SpawnLoc);

	const int32 NumProjectiles = ProjectileNums.GetValueAtLevel(GetAbilityLevel());
	const float DeltaSpread = ProjectileSpread / NumProjectiles;
	FRotator RotatingRot = InRot - FRotator(0.f, ProjectileSpread / 2.f, 0.f);

	const bool bHasAuth = HasAuthorityOrPredictionKey(GetCurrentActorInfo(), &GetCurrentActivationInfoRef());
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
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn /*can spawn inside others, HitResult.Distance == 0.f*/);
			Projectile->SpawnedFromAbility = this;
			Projectile->FinishSpawning(SpawnTransform);
			if (bHomingValid) Projectile->MulticastSetHomingTarget(HomingTarget->GetRootComponent(), HomingAcceleration);
		}
		else if (bPredictingClient)
		{
			bool bSpawn = false;
			/*AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform,
				nullptr /*Owned by the staff, but it's not an actor#1#, AuraCharacter,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn /*can spawn inside other actor, HitResult.Distance == 0.f#1#);
			Projectile->SpawnedFromAbility = this;*/
		}
	}
}



// AbilityTask ======================================================================================================
UAbilityTask_SpawnProjectile* UAbilityTask_SpawnProjectile::SpawnProjectile(UProjectileAbility* OwningAbility,
	const ECombatSocket SpawnSocket, const FVector EndPoint, AActor* Target, const float HeightIfHitGround)
{
	UAbilityTask_SpawnProjectile* Task = NewAbilityTask<UAbilityTask_SpawnProjectile>(OwningAbility);
	Task->ProjectileAbility = OwningAbility;
	Task->SocketLocation = Task->ProjectileAbility->AuraCharacter->GetCombatSocketLocation(SpawnSocket);
	Task->EndPoint = EndPoint;
	Task->Target = Target;
	Task->HeightIfHitGround = HeightIfHitGround;
	return Task;
}

void UAbilityTask_SpawnProjectile::Activate()
{
	if (IsLocallyControlled())
	{
		// Struct that is not meant to be used, automatically finish when out of scope. REQUIRED to set Prediction
		// Player input SHOULD be instantly predicted (e.g. 'hold down and charge')
		FScopedPredictionWindow(AbilitySystemComponent.Get());
		if (IsPredictingClient())
		{
			FGA_TargetData_ProjectileInfo* Data = new FGA_TargetData_ProjectileInfo();
			Data->Location = SocketLocation;
			Data->Rotation = ProjectileAbility->AuraCharacter->AimDirection.ToOrientationRotator();
			Data->ActivationTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
			Data->EndPoint = EndPoint;
			FGameplayAbilityTargetDataHandle DataHandle(Data);

			AbilitySystemComponent->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(),
				GetActivationPredictionKey(), DataHandle, FGameplayTag(),
				AbilitySystemComponent->ScopedPredictionKey);
		}
		else
		{	// Server
			ProjectileAbility->SpawnProjectile(SocketLocation, EndPoint,
				ProjectileAbility->AuraCharacter->AimDirection.ToOrientationRotator(), Target, HeightIfHitGround);
		}
		if (ShouldBroadcastAbilityTaskDelegates()) OnSpawnFinish.Broadcast();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

		AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(
			this, &UAbilityTask_SpawnProjectile::OnTargetDataReplicatedCallback);
		if (!AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey))
		{
			SetWaitingOnRemotePlayerData(); // if data hasn't reached the server yet
		}
	}
}

void UAbilityTask_SpawnProjectile::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	const FGameplayAbilityTargetData* Data = DataHandle.Get(0);
	const FTransform ClientTransformData = Data->GetOrigin();
	const FVector ClientSpawnLocation = ClientTransformData.GetTranslation();
	FVector Difference = ClientSpawnLocation - SocketLocation;
	const float DifferenceSizeSqr = Difference.SizeSquared();
	if (DifferenceSizeSqr > 0.f)
	{
		if (DifferenceSizeSqr < 10'000.f) SocketLocation = ClientSpawnLocation;
		else if (Difference.Normalize()) SocketLocation += Difference * 100.f;
	}
	ProjectileAbility->AuraCharacter->AimDirection = ClientTransformData.GetRotation().GetAxisX();
	const FVector Floats = ClientTransformData.GetScale3D(); // Arbitrary numbers are stored in Scale
	const float TimeDifferent = GetWorld()->GetGameState()->GetServerWorldTimeSeconds() - Floats.X;
	if (TimeDifferent > UE_KINDA_SMALL_NUMBER && TimeDifferent < .5f)
	{
		// Modify Spawn according to time difference here
	}
	EndPoint = Data->GetEndPoint();
	ProjectileAbility->SpawnProjectile(SocketLocation, EndPoint,
		ProjectileAbility->AuraCharacter->AimDirection.ToOrientationRotator(), Target, HeightIfHitGround);

	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates()) OnSpawnFinish.Broadcast();
}
