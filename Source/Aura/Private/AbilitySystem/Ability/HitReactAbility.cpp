// Copyright Hung


#include "AbilitySystem/Ability/HitReactAbility.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityLibrary.h"
#include "AuraTag.h"
#include "AbilitySystem/Ability/DamageAbility.h"
#include "Character/AuraCharacterBase.h"
#include "Components/CapsuleComponent.h"

UHitReactAbility::UHitReactAbility()
{
	SetAssetTags(FGameplayTagContainer(AuraTag::State_HitReact)); // so that Death can cancel this
	CancelAbilitiesWithTag = FGameplayTagContainer(AuraTag::Ability_Cancelable_Generic);
	ActivationOwnedTags.AddTag(AuraTag::State_HitReact);
	ActivationOwnedTags.AddTag(AuraTag::State_Block_Movement_Speed);
	ActivationOwnedTags.AddTag(AuraTag::State_Block_Movement_Rotation);
	ActivationBlockedTags.AddTag(AuraTag::State_Death);

	bRetriggerInstancedAbility = true;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;

	FAbilityTriggerData& Data = AbilityTriggers.AddDefaulted_GetRef();
	Data.TriggerTag = AuraTag::State_HitReact;
	Data.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
}

void UHitReactAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{	// if using ActivateAbility, beware that Super::ActivateAbility calls BP_ActivateAbility inside so do it before Super
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (TriggerEventData == nullptr) return;
	const FGameplayAbilityTargetData* Data = TriggerEventData->TargetData.Get(0);
	if (!Data || Data->GetScriptStruct() != FGATargetData_HitReact::StaticStruct()) return;
	const FGATargetData_HitReact* HitReactData = static_cast<const FGATargetData_HitReact*>(Data);

	HitReactTags = MoveTemp(const_cast<FGameplayTagContainer&>(TriggerEventData->TargetTags));
	if (HitReactTags.HasTag(AuraTag::State_HitReact_PlayMontage))
	{
		OnHitReact();
	}

	if (HitReactTags.HasTag(AuraTag::State_HitReact_Knockback) && HitReactData->KnockbackDistance > 0)
	{
		float Duration = HitReactData->KnockbackTime;
		const FVector StartLoc = AuraCharacter->GetActorLocation();
		const UCapsuleComponent* CapsuleComp = AuraCharacter->GetCapsuleComponent();
		FVector MoveToLoc = StartLoc + HitReactData->KnockbackDirection * HitReactData->KnockbackDistance;
		/** TODO: Split Mesh into Nav floor and Wall for knockback, make profiles for wall (mostly same as Nav floor)
		 * or use different PhysMat
		 * - Then UKismetSystemLibrary::CapsuleTraceSingleByProfile*/
		TArray<FHitResult> OutHits;
		const bool bHit = UAuraAbilityLibrary::TraceByChannel(this, StartLoc, MoveToLoc,
			{AuraCharacter}, KnockbackDebug, OutHits, {ECC_WorldStatic},
			CapsuleComp->GetScaledCapsuleRadius());

		for (const FHitResult& Hit : OutHits)
		{
			if (Hit.bBlockingHit)
			{
				MoveToLoc = Hit.Location;
				Duration *= Hit.Distance / HitReactData->KnockbackDistance;
			}
			break; // only get the first hit
		}

		if (HasAuthority(&CurrentActivationInfo))
		{
			Knockback(MoveToLoc, Duration, bHit); // AuraCharacter->LaunchCharacter();
		}
	}
}

void UHitReactAbility::TryEndHitReact(const FGameplayTag Tag)
{
	HitReactTags.RemoveTag(Tag);
	if (HitReactTags.Num() == 0)
	{
		K2_EndAbility();
	}
}
