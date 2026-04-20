// Copyright Hung


#include "AbilitySystem/Ability/DamageAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilityLibrary.h"
#include "AuraTag.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/Ability/AttributesEventAbility.h"
#include "Character/AuraCharacterBase.h"

TArray<FGameplayEffectSpecHandle> UDamageAbility::MakeOutgoingAbilityEffectsSpecs()
{
	const int32 Level = GetAbilityLevel();
	TArray<FGameplayEffectSpecHandle> SpecHandles;
	for (const auto& [EffectClass, Period,
		Magnitudes] : AbilityEffects)
	{
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass, Level);
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		if (Period > .03f) Spec->Period = Period;
		for (auto& [Tag, ScalableFloat] : Magnitudes)
		{
			if (FMath::IsNearlyZero(ScalableFloat.Value))
			{	// If 0, Is Granted DebuffTag
				Spec->DynamicGrantedTags.AddTag(Tag);
			}
			else
			{
				if (true) {} // Do check Inventory / Upgrades for GetValueAtLevel([UpgradeNum]); instead of using Level
				Spec->SetByCallerTagMagnitudes.FindOrAdd(Tag) = ScalableFloat.GetValueAtLevel(Level);
			}
		}
		SpecHandles.Add(SpecHandle);
	}
	return MoveTemp(SpecHandles);
}

void UDamageAbility::ApplyAbilityEffectsToTarget(const AActor* InTarget, TArray<FGameplayEffectSpecHandle>& SpecHandles,
	const FVector& InDirection)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InTarget);
	if (!TargetASC) return;
	for (const FGameplayEffectSpecHandle& Handle : SpecHandles)
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*Handle.Data.Get());
	}

	if (FMath::RandRange(UE_SMALL_NUMBER, 1.f) < KnockbackChance)
	{
		FGameplayEventData HitReactPayload;
		HitReactPayload.TargetTags.AddTag(AuraTag::State_HitReact_Knockback);
		FGATargetData_HitReact* Data = new FGATargetData_HitReact();
		*Data = HitReactTargetData;
		Data->KnockbackDirection = InDirection.IsNearlyZero() ?
			InTarget->GetActorLocation() - AuraCharacter->GetActorLocation() : InDirection;
		Data->KnockbackDirection.Normalize();
		if (HitWallEffect.EffectClass)
		{
			Data->HitWallEffectSpecHandle = MakeOutgoingGameplayEffectSpec(HitWallEffect.EffectClass, GetAbilityLevel());
		}
		HitReactPayload.TargetData = FGameplayAbilityTargetDataHandle(Data);
		TargetASC->HandleGameplayEvent(AuraTag::State_HitReact, &HitReactPayload);
	}
}

void UDamageAbility::MeleeTraceAndApplyEffects(const float Radius, const FVector& InLoc, const FGameplayTag& ImpactCue,
	const EDrawDebugTrace::Type DrawDebugType)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr) return;

	const FVector ActorLoc = AuraCharacter->GetActorLocation();
	const FVector Start = ActorLoc + (InLoc - ActorLoc) * .33f;

	TArray<FHitResult> HitResults;
	UAuraAbilityLibrary::TraceByChannel(this, Start, InLoc, {AuraCharacter},
		HitResults, {ECC_AuraTrace_Effect}, Radius, true, false, DrawDebugType);

	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	const float Level = GetAbilityLevel();
	for (FHitResult& Hit : HitResults)
	{
		if (!Hit.PhysMaterial.Get()) continue; // ignore collision with capsule comp
		AActor* Actor = Hit.GetActor();
		if (UAuraAbilityLibrary::IsAlly(Actor , AuraCharacter)) continue;

		if (ImpactCue.IsValid())
		{	// Allow cue on wall and dead body
			FGameplayCueParameters Params; Params.AbilityLevel = Level;
			Params.Location = Hit.ImpactPoint.IsNearlyZero() ? Actor->GetActorLocation() : Hit.ImpactPoint;
			Params.Normal = Hit.ImpactNormal;
			Params.PhysicalMaterial = Hit.PhysMaterial; // Set in PhysicalAsset/Component Collision/Material
			ASC->ExecuteGameplayCue(ImpactCue, Params);
		}

		TArray<FGameplayEffectSpecHandle> SpecHandles = MakeOutgoingAbilityEffectsSpecs();
		for (const FGameplayEffectSpecHandle SpecHandle : SpecHandles)
		{
			SpecHandle.Data->GetContext().AddHitResult(Hit);
		}

		FVector Direction = Actor->GetActorLocation() - ActorLoc;
		Direction.Z = 0.f;
		ApplyAbilityEffectsToTarget(Actor, SpecHandles, Direction);
	}
}

bool UDamageAbility::ExecuteCueShowDamage(const FGameplayCueParameters& Parameters, bool& bBlocked, bool& bCrit)
{
	/*const AActor* SourcePS = Parameters.EffectContext.GetInstigator(); if (SourcePS == nullptr) return false;
	const AAuraPlayerController* PC = Cast<AAuraPlayerController>(SourcePS->GetOwner());
	if (PC == nullptr || GEngine == nullptr) return false;
	const APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(PC->GetWorld());
	if (LocalPlayerController == nullptr || LocalPlayerController != PC) return false; // if damage dealer isn't local*/

	bBlocked = Parameters.AggregatedTargetTags.HasTag(AuraTag::Damage_Blocked);
	bCrit = Parameters.AggregatedTargetTags.HasTag(AuraTag::Damage_Crit);

	if (Parameters.RawMagnitude < UE_KINDA_SMALL_NUMBER) return false;
	return true; // Play sound and spawn niagara but don't show text dmg if not set
}

void UDamageAbility::GetAbilityDetails(FAbilityDetails& Details) const
{
	Super::GetAbilityDetails(Details);
	for (const FAbilityEffect& Effect : AbilityEffects)
	{
		for (const auto& [Tag, ScalableFloat] : Effect.SetByCallerTagMagnitudes)
		{
			if (!AuraTag::DamageTypeContainer.HasTag(Tag)) continue;
			Details.Damage = static_cast<int32>(ScalableFloat.GetValueAtLevel(Details.Level) * 10.f) / 10.f;
		}
	}
}
