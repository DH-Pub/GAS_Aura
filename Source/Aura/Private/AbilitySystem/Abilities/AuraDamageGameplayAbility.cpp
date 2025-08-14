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

UAuraDamageGameplayAbility::UAuraDamageGameplayAbility()
{
	FGameplayTagContainer DefaultTags(AuraGameplayTags::Ability_Attack);
	DefaultTags.AddTag(AuraGameplayTags::Ability_Type_Activatable_Blockable);
	SetAssetTags(DefaultTags);
	BlockAbilitiesWithTag = FGameplayTagContainer(AuraGameplayTags::Ability_Type_Activatable_Blockable);
}

void UAuraDamageGameplayAbility::CauseDamageToActors(FGameplayTag GameplayCueTag, const TArray<AActor*>& Actors,
	USoundBase* ImpactSound, bool bStagger)
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
