// Copyright Hung


#include "AbilitySystem/Abilities/DamageAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/AuraCharacterBase.h"
#include "StructUtils/InstancedStruct.h"

float UDamageAbility::GetDamageAtLevel(const UDamageAbility* Ability, const int32 Level, const FGameplayTag TypeTag)
{
	if (const FScalableFloat* ScalableFloat = Ability->DamageTypes.Find(TypeTag))
	{
		return static_cast<int32>(ScalableFloat->GetValueAtLevel(Level) * 10.f) / 10.f;
	}
	return 0.f;
}

void UDamageAbility::CauseDamageToActors(const FGameplayTag GameplayCueTag, const TArray<AActor*>& Actors, USoundBase* ImpactSound)
{
	if (Actors.IsEmpty()) return;
	UAbilitySystemComponent* SourceASC = GetCurrentActorInfo()->AbilitySystemComponent.Get();
	FGameplayEffectContextHandle EffectContextHandle = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
	EffectContextHandle.AddSourceObject(AuraCharacterFromActorInfo);
	UAuraLibrary::SetIsStaggerDamage(EffectContextHandle, bStagger);
	FGameplayEffectSpecHandle GESpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, EffectContextHandle);
	FDamageCueList DamageCueList;
	for (AActor* Actor : Actors)
	{
		if (!UAuraLibrary::IsNotFriend(GetAvatarActorFromActorInfo(), Actor)) continue;
		for (const TPair<FGameplayTag, FScalableFloat>& Pair: DamageTypes)
		{
			GESpecHandle.Data.Get()->SetByCallerTagMagnitudes.FindOrAdd(Pair.Key) = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		}
		SourceASC->ApplyGameplayEffectSpecToTarget(*GESpecHandle.Data, UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor));
		
		// GameplayAbility only replicated on "Owning Client" and/or "Server", Effect Spawned here WON'T show on others => use GameplayCue
		DamageCueList.DamageCues.Add(
			FDamageCue(Actor->GetActorLocation(), ImpactSound,
			Actor->Implements<UCombatInterface>() ? ICombatInterface::Execute_GetBloodEffect(Actor) : nullptr));
	}
	
	const FInstancedStruct InstancedStruct = FInstancedStruct::Make(DamageCueList);
	UAuraLibrary::SetInstancedStruct(EffectContextHandle, InstancedStruct);
	SourceASC->ExecuteGameplayCue(GameplayCueTag, FGameplayCueParameters(EffectContextHandle));
}

void UDamageAbility::MeleeImpactCueFromEffectContext(const UObject* WorldContextObject,
	const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FInstancedStruct* InstancedStruct = UAuraLibrary::GetInstancedStructPointer(EffectContextHandle))
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
