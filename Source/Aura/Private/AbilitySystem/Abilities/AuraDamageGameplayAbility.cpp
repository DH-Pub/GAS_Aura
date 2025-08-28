// Copyright Hung


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/AuraCharacterBase.h"
#include "StructUtils/InstancedStruct.h"

float UAuraDamageGameplayAbility::GetDamageAtLevel(const int32 Level, const FGameplayTag TypeTag)
{
	if (const FScalableFloat* ScalableFloat = DamageTypes.Find(TypeTag))
	{
		return static_cast<int32>(ScalableFloat->GetValueAtLevel(Level) * 10.f) / 10.f;
	}
	return 0.f;
}
void UAuraDamageGameplayAbility::GetDamageAtLevelChanged(float& Damage, float& DamageChanged,
	const FGameplayTag TypeTag, const int32 Level, const int32 LevelDelta)
{
	if (const FScalableFloat* ScalableFloat = DamageTypes.Find(TypeTag))
	{
		Damage = static_cast<int32>(ScalableFloat->GetValueAtLevel(Level) * 10.f) / 10.f;
		DamageChanged = static_cast<int32>(ScalableFloat->GetValueAtLevel(Level + LevelDelta) * 10.f) / 10.f;
	}
}

void UAuraDamageGameplayAbility::CauseDamageToActors(const FGameplayTag GameplayCueTag, const TArray<AActor*>& Actors,
                                                     USoundBase* ImpactSound, const bool bStagger)
{
	if (Actors.IsEmpty()) return;
	UAbilitySystemComponent* SourceASC = GetCurrentActorInfo()->AbilitySystemComponent.Get();
	FGameplayEffectContextHandle EffectContextHandle = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
	EffectContextHandle.AddSourceObject(AuraCharacterFromActorInfo);
	UAuraAbilitySystemLibrary::SetIsStaggerDamage(EffectContextHandle, bStagger);
	FGameplayEffectSpecHandle GESpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, EffectContextHandle);
	FDamageCueList DamageCueList;
	for (AActor* Actor : Actors)
	{
		// GESpecHandle.Data->GetContext().AddOrigin(Actor->GetActorLocation()); // for showing dmg // DEPRECATED: Using ActorLocation
		if (!UAuraAbilitySystemLibrary::IsNotFriend(GetAvatarActorFromActorInfo(), Actor)) continue;
		for (TPair Pair: DamageTypes)
		{
			const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(GESpecHandle, Pair.Key, ScaledDamage);
		}
		SourceASC->ApplyGameplayEffectSpecToTarget(*GESpecHandle.Data, UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor));
		
		// GameplayAbility only replicated on "Owning Client" and/or "Server", Effect Spawned here WON'T show on others => use GameplayCue
		DamageCueList.DamageCues.Add(
			FDamageCue(Actor->GetActorLocation(), ImpactSound,
			Actor->Implements<UCombatInterface>() ? ICombatInterface::Execute_GetBloodEffect(Actor) : nullptr));
	}
	
	const FInstancedStruct InstancedStruct = FInstancedStruct::Make(DamageCueList);
	UAuraAbilitySystemLibrary::SetInstancedStruct(EffectContextHandle, InstancedStruct);
	SourceASC->ExecuteGameplayCue(GameplayCueTag, FGameplayCueParameters(EffectContextHandle));
}

void UAuraDamageGameplayAbility::MeleeImpactCueFromEffectContext(const UObject* WorldContextObject,
	const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FInstancedStruct* InstancedStruct = UAuraAbilitySystemLibrary::GetInstancedStructPointer(EffectContextHandle))
	{
		if (const FDamageCueList* DamageCueList = InstancedStruct->GetPtr<FDamageCueList>())
		{
			for(const auto& [Location, EffectSound, NiagaraSystem]
				: DamageCueList->DamageCues)
			{
				UGameplayStatics::PlaySoundAtLocation(WorldContextObject, EffectSound, Location);
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, NiagaraSystem, Location);
			}
		}
	}
}
