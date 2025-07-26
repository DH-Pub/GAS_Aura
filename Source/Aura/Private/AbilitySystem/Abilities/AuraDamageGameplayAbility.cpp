// Copyright Hung


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayEffectTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

UAuraDamageGameplayAbility::UAuraDamageGameplayAbility()
{
	FGameplayTagContainer DamageAssetTags(AuraGameplayTags::Abilities_Attack);
	SetAssetTags(DamageAssetTags);
}

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	for (TPair<FGameplayTag, FScalableFloat> Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel()); 
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data,
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

void UAuraDamageGameplayAbility::CauseDamageToActors(TArray<AActor*> Actors, const FGameplayTag GameplayCueTag,
	FGameplayCueParameters& GameplayCueParameters, const bool bStagger)
{
	if (Actors.IsEmpty()) return;
	UAbilitySystemComponent* SourceASC = GetCurrentActorInfo()->AbilitySystemComponent.Get();
	FGameplayEffectContextHandle EffectContextHandle = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
	EffectContextHandle.AddSourceObject(AvatarActor);
	UAuraAbilitySystemLibrary::SetIsStaggerDamage(EffectContextHandle, bStagger);
	FGameplayEffectSpecHandle GESpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, EffectContextHandle);
	for (AActor* Actor : Actors)
	{
		// GESpecHandle.Data->GetContext().AddOrigin(Actor->GetActorLocation()); // for showing dmg // DEPRECATED: Using ActorLocation
		if (UAuraAbilitySystemLibrary::IsNotFriend(AvatarActor, Actor))
		{
			for (TPair Pair: DamageTypes)
			{
				const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
				UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(GESpecHandle, Pair.Key, ScaledDamage);
			}
			SourceASC->ApplyGameplayEffectSpecToTarget(*GESpecHandle.Data, UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor));
			GameplayCueParameters.EffectContext = EffectContextHandle;
			// GameplayCueParameters.Instigator = AvatarActor;
			GameplayCueParameters.EffectCauser = AvatarActor;
			GameplayCueParameters.SourceObject = Actor;
			SourceASC->ExecuteGameplayCue(GameplayCueTag, GameplayCueParameters);
			/* GameplayAbility only replicated on "Owning Client" and "Server"
			 * UNiagaraFunctionLibrary::SpawnSystemAtLocation(Actor, ICombatInterface::Execute_GetBloodEffect(Actor), Actor->GetActorLocation());
			 * The above effects won't show up on others
			 */
		}
	}
}
