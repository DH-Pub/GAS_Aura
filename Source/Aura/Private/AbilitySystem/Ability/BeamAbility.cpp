// Copyright Hung


#include "AbilitySystem/Ability/BeamAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilityLibrary.h"
#include "Actor/AuraBeam.h"
#include "Character/AuraCharacterBase.h"
#include "Kismet/GameplayStatics.h"

UBeamAbility::UBeamAbility()
{
	DrawDebug = EDrawDebugTrace::None;
}

void UBeamAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	CommitAbilityCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false);
	GetWorld()->GetTimerManager().ClearTimer(BeamTickTimerHandle);
	if (Beam) Beam->Destroy();
}

void UBeamAbility::StartBeam(USceneComponent* BeamFromComp, const ECombatSocket StartSocketName)
{
	if (!HasAuthority(&CurrentActivationInfo)) return;

	FTransform SpawnTransform;
	Beam = GetWorld()->SpawnActorDeferred<AAuraBeam>(BeamClass, SpawnTransform, nullptr,
		AuraCharacter, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	// FAttachmentTransformRules Rules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
	Beam->AttachToComponent(BeamFromComp, FAttachmentTransformRules(EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true),
		AAuraCharacterBase::GetCombatSocketName(StartSocketName));
	Beam->FinishSpawning(SpawnTransform);

	GetWorld()->GetTimerManager().SetTimer(BeamTickTimerHandle, this, &UBeamAbility::BeamTraceForTarget,
		BeamTickInterval, true, 0.f); // delay 0.f to start immediately
}

void UBeamAbility::BeamTraceForTarget()
{
	if (!CommitAbilityCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{	// Replicate End to Stop animation on Client side
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	const FVector CharacterLoc = AuraCharacter->GetActorLocation();
	const float Length = BeamLength.GetValueAtLevel(GetAbilityLevel());
	const FVector End = CharacterLoc + AuraCharacter->AimDirection * Length;
	TArray<AActor*> ActorsToIgnore = {AuraCharacter};
	static TArray<AActor*> Actors; Actors.Reset();
	UAuraAbilityLibrary::ConeOverlapLivingCharacters(this, CharacterLoc, AuraCharacter->AimDirection,
		Length, AngleDeg, ActorsToIgnore, DrawDebug, Actors);

	float DistanceSqr = MAX_flt;
	AActor* FirstActor = nullptr;
	for (int32 i = 0; i < Actors.Num(); i++)
	{
		AActor* Actor = Actors[i];
		if (UAuraAbilityLibrary::IsAlly(AuraCharacter, Actor)) continue;
		const float HitDistSqr = (Actor->GetActorLocation() - CharacterLoc).SizeSquared();
		if (HitDistSqr < DistanceSqr)
		{
			DistanceSqr = HitDistSqr;
			FirstActor = Actor;
		}
	}

	FGATargetData_BeamData BeamData;
	if (FirstActor)
	{
		BeamData.TargetActorArray.AddUnique(FirstActor);

		ChainBeamOverlaps(MaxChained.GetValueAtLevel(GetAbilityLevel()), ActorsToIgnore, Length,
			BeamData);

		FVector PrevActorLoc = AuraCharacter->GetActorLocation();
		for (const TWeakObjectPtr<AActor>& Actor : BeamData.TargetActorArray)
		{	// Apply DAMAGE ====================================================================================
			const FVector ChainLoc = Actor->GetActorLocation(); // make Spec for each target to not get overriden
			TArray<FGameplayEffectSpecHandle> SpecHandles = MakeOutgoingAbilityEffectsSpecs();
			for (const FGameplayEffectSpecHandle& SpecHandle : SpecHandles)
			{
				SpecHandle.Data->GetContext().AddOrigin(Actor->GetActorLocation());
			}
			ApplyAbilityEffectsToTarget(Actor.Get(), SpecHandles, ChainLoc - PrevActorLoc);
			PrevActorLoc = ChainLoc;
		}
	}
	else // Found nothing to Beam, Just Hit any blocking object
	{
		FHitResult Hit;
		if (UAuraAbilityLibrary::TraceSingleByChannel(this, Hit, CharacterLoc, End,
			ECC_Visibility, ActorsToIgnore, 0, DrawDebug))
		{
			BeamData.SourceLocation.Add(Hit.ImpactPoint);
		}
		else BeamData.SourceLocation.Add(End); // Use end point if Trace doesn't hit anything
	}

	if (Beam) Beam->MulticastSetTarget(BeamData);
}
void UBeamAbility::ChainBeamOverlaps(int32 ChainBeamRemains, TArray<AActor*>& InActorsToIgnore, const float Length,
	FGATargetData_BeamData& BeamData) const
{
	while (ChainBeamRemains > 0)
	{
		InActorsToIgnore.AddUnique(BeamData.TargetActorArray.Last().Get());
		const AActor* LastActor = InActorsToIgnore.Last();
		const FVector LastActorLoc = LastActor->GetActorLocation();
		static TArray<AActor*> OverlapCharas; OverlapCharas.Reset();
		UAuraAbilityLibrary::GetLiveCharactersInRadius(AuraCharacter, OverlapCharas, InActorsToIgnore,
			Length, LastActorLoc);
		if (OverlapCharas.Num() == 0) break;
		UAuraAbilityLibrary::FilterOutAllies(AuraCharacter, OverlapCharas);

		float Dist;
		if (AActor* Actor = UGameplayStatics::FindNearestActor(LastActorLoc, OverlapCharas, Dist))
		{
			BeamData.TargetActorArray.AddUnique(Actor);
			ChainBeamRemains--; // decrement IF there is a Target to set for EffectCauser
			continue;
		}
		break;
	}
}
