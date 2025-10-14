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
	return SpecHandle; // return FGameplayEffectSpec* will cause error
}

void UDamageAbility::CauseDamageToActors(const TArray<AActor*>& Actors, USoundBase* ImpactSound)
{
	if (Actors.IsEmpty() || !HasAuthority(&GetCurrentActivationInfoRef())) return;
	FGameplayEffectSpecHandle SpecHandle = MakeDamageSpecHandle();
	FAuraEffectContext* AuraContext = FAuraEffectContext::ExtractAuraContext(SpecHandle.Data->GetContext().Get());
	FDamageEffectContext& DamageContext = FAuraEffectContext::GetOrMakeContextStructRef<FDamageEffectContext>(AuraContext);
	for (AActor* Actor : Actors)
	{
		if (!UAuraAbilityLibrary::IsNotFriend(GetAvatarActorFromActorInfo(), Actor)) continue;
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
		{
			if (TargetASC->HasMatchingGameplayTag(AuraGameplayTags::Character_State_Death)) continue;
			AuraContext->AddOrigin(Actor->GetActorLocation());
			DamageContext.DamageDirection =
				(Actor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).GetSafeNormal();
			DamageContext.ImpactSound = ImpactSound;
			DamageContext.ImpactEffect = Actor->Implements<UCombatInterface>() ?
				ICombatInterface::Execute_GetBloodEffect(Actor) : nullptr;
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
	}
}

bool UDamageAbility::ExecuteCueShowDamage(const FGameplayCueParameters& Parameters,
	float& OutDamage, FDamageEffectContext& OutDamageContext, FVector& OutLoc)
{
	/*const AActor* SourcePS = Parameters.EffectContext.GetInstigator(); if (SourcePS == nullptr) return false;
	const AAuraPlayerController* PC = Cast<AAuraPlayerController>(SourcePS->GetOwner());
	if (PC == nullptr || GEngine == nullptr) return false;
	const APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(PC->GetWorld());
	if (LocalPlayerController == nullptr || LocalPlayerController != PC) return false; // if damage dealer isn't local*/

	const FAuraEffectContext* AuraContext = FAuraEffectContext::ExtractAuraEffectContext(Parameters.EffectContext);
	if (!AuraContext->IsShowDamageOnTarget()) return false;
	OutDamage = Parameters.RawMagnitude;
	if (OutDamage < UE_SMALL_NUMBER) return false;
	if (const FDamageEffectContext* Context = AuraContext->GetStruct<FDamageEffectContext>())
	{
		OutDamageContext = *Context;
		OutLoc = Parameters.EffectContext.GetOrigin();
		UGameplayStatics::PlaySoundAtLocation(Parameters.GetInstigator(), Context->ImpactSound, OutLoc);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(Parameters.GetInstigator(), Context->ImpactEffect, OutLoc);
	}
	else
	{
		OutDamageContext = FDamageEffectContext(Parameters.EffectContext.GetEffectCauser());
		OutLoc = OutDamageContext.TargetActor->GetActorLocation();
	}
	return true;
}


UDamageGameplayEffect::UDamageGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	UDebuffComponent* DebuffComponent = CreateDefaultSubobject<UDebuffComponent>("Debuff");
	GEComponents.Add(DebuffComponent);
	// DebuffComponent.OnCompleteDamageDebuff;
	FGameplayEffectExecutionDefinition ExecDef; ExecDef.CalculationClass = UExecCalc_Damage::StaticClass();
	Executions.Add(ExecDef);
	FGameplayEffectCue Cue(AuraGameplayTags::GameplayCue_Damage, 0.f, 0.f);
	Cue.MagnitudeAttribute = UAuraAttributeSet::GetIncomingDamageAttribute();
	GameplayCues.Add(Cue);
}

UDebuffDamageEffect::UDebuffDamageEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FGameplayEffectExecutionDefinition ExecDef; ExecDef.CalculationClass = UExecCalc_Debuff::StaticClass();
	Executions.Add(ExecDef);

	FGameplayEffectCue Cue(AuraGameplayTags::GameplayCue_Damage, 0.f, 0.f);
	Cue.MagnitudeAttribute = UAuraAttributeSet::GetIncomingDamageAttribute();
	GameplayCues.Add(Cue);

	StackingType = EGameplayEffectStackingType::AggregateByTarget;
}
