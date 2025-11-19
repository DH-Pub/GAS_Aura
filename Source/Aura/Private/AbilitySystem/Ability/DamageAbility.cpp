// Copyright Hung


#include "AbilitySystem/Ability/DamageAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilityLibrary.h"
#include "AuraEffectTypes.h"
#include "AuraGameplayTags.h"
#include "Interface/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystem/ExecCalc/ExecCalc_Debuff.h"
#include "AbilitySystem/GameplayEffectComponent/DebuffComponent.h"

FGameplayEffectSpecHandle UDamageAbility::MakeDamageSpecHandle() const
{
	ensureMsgf(IsInstantiated(), TEXT("%hs called on the CDO. "
		"This function uses instance variables and therefore is invalid on the CDO."), __func__);
	if (DamageEffectClass == nullptr) return FGameplayEffectSpecHandle();
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	Spec->SetByCallerTagMagnitudes.FindOrAdd(DamageType) = DamageValue.GetValueAtLevel(GetAbilityLevel());
	Spec->SetDuration(DebuffDelay, true);
	Spec->Period = Spec->Duration + 10.f;
	if (bStagger) Spec->AddDynamicAssetTag(AuraGameplayTags::Character_State_HitReact);
	FAuraEffectContext::SetIsShowDamageOnTarget(Spec->GetContext().Get(), true);
	FDamageEffectContext* DamageContext = FAuraEffectContext::MakeStructInContext<FDamageEffectContext>(
		SpecHandle.Data->GetContext());
	DamageContext->bKnockback = FMath::RandRange(UE_SMALL_NUMBER, 1.f) < KnockbackChance; // Target of this Ability does not have pointer access to this Ability
	DamageContext->KnockbackForce = KnockbackForce;
	return SpecHandle; // return FGameplayEffectSpec* will cause error
}

FActiveGameplayEffectHandle UDamageAbility::ApplyDebuffToTarget(UAbilitySystemComponent* TargetASC) const
{
	if (DebuffEffectClass == nullptr) return FActiveGameplayEffectHandle();
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DebuffEffectClass, GetAbilityLevel());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	Spec->DynamicGrantedTags.AddTag(DebuffType);
	Spec->SetDuration(DebuffDuration.GetValueAtLevel(Spec->GetLevel()), true);
	Spec->Period = DebuffPeriod.GetValueAtLevel(GetAbilityLevel());
	FAuraEffectContext::SetIsShowDamageOnTarget(Spec->GetContext().Get(), true);
	return TargetASC->ApplyGameplayEffectSpecToSelf(*Spec);
}

void UDamageAbility::CauseDamageToActors(const TArray<AActor*>& Actors, USoundBase* ImpactSound)
{
	//TODO: Use FGameplayAbilityTargetData
	if (Actors.IsEmpty() || !HasAuthority(&CurrentActivationInfo)) return;
	for (AActor* Actor : Actors)
	{
		if (!UAuraAbilityLibrary::IsNotFriend(GetAvatarActorFromActorInfo(), Actor)) continue;
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
		{
			if (TargetASC->HasMatchingGameplayTag(AuraGameplayTags::Character_State_Death)) continue;
			FGameplayEffectSpecHandle SpecHandle = MakeDamageSpecHandle();
			FDamageEffectContext* DamageContext = FAuraEffectContext::GetOrMakeContextStructPtr<FDamageEffectContext>(
				SpecHandle.Data->GetContext());
			SpecHandle.Data->GetContext().AddOrigin(Actor->GetActorLocation());
			DamageContext->DamageDirection =
				(Actor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).GetSafeNormal();
			DamageContext->ImpactSound = ImpactSound;
			DamageContext->ImpactEffect = Actor->Implements<UCombatInterface>() ?
				ICombatInterface::Execute_GetBloodEffect(Actor) : nullptr;
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
	}
}

bool UDamageAbility::ExecuteCueShowDamage(const FGameplayCueParameters& Parameters,
	float& OutDamage, FDamageEffectContext& OutDamageContext, FVector& OutLoc)
{
	AActor* Instigator = Parameters.GetInstigator();
	/*const AActor* SourcePS = Parameters.EffectContext.GetInstigator(); if (SourcePS == nullptr) return false;
	const AAuraPlayerController* PC = Cast<AAuraPlayerController>(SourcePS->GetOwner());
	if (PC == nullptr || GEngine == nullptr) return false;
	const APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(PC->GetWorld());
	if (LocalPlayerController == nullptr || LocalPlayerController != PC) return false; // if damage dealer isn't local*/

	const FAuraEffectContext* AuraContext = FAuraEffectContext::ExtractAuraContext(Parameters.EffectContext);
	if (!AuraContext->IsShowDamageOnTarget()) return false;
	OutDamage = Parameters.RawMagnitude;
	if (OutDamage < UE_SMALL_NUMBER) return false;
	OutDamageContext = *AuraContext->GetStruct<FDamageEffectContext>();
	OutLoc = Parameters.EffectContext.HasOrigin() ?
		Parameters.EffectContext.GetOrigin() : OutDamageContext.TargetActor->GetActorLocation();
	UGameplayStatics::PlaySoundAtLocation(Instigator, OutDamageContext.ImpactSound, OutLoc);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(Instigator, OutDamageContext.ImpactEffect, OutLoc);
	return true;
}


// ========================================== Effect ===========================================================
#pragma region Effect
UDamageGameplayEffect::UDamageGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	UDebuffComponent* DebuffComponent = CreateDefaultSubobject<UDebuffComponent>("Debuff");
	GEComponents.Add(DebuffComponent); // DebuffComponent.OnCompleteDamageDebuff;

	int32 i = Executions.Add(FGameplayEffectExecutionDefinition());
	Executions[i].CalculationClass = UExecCalc_Damage::StaticClass();

	i = GameplayCues.Add(FGameplayEffectCue(AuraGameplayTags::GameplayCue_Damage, 0.f, 0.f));
	GameplayCues[i].MagnitudeAttribute = UAuraAttributeSet::GetIncomingDamageAttribute();
}

UDebuffDamageEffect::UDebuffDamageEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	int32 i = Executions.Add(FGameplayEffectExecutionDefinition());
	Executions[i].CalculationClass = UExecCalc_Debuff::StaticClass();

	i = GameplayCues.Add(FGameplayEffectCue(AuraGameplayTags::GameplayCue_Damage, 0.f, 0.f));
	GameplayCues[i].MagnitudeAttribute = UAuraAttributeSet::GetIncomingDamageAttribute();

	StackingType = EGameplayEffectStackingType::AggregateByTarget;
}
#pragma endregion
