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
	SetAssetTags(FGameplayTagContainer(AuraGameplayTags::Abilities_Attack));
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
		FDamageCue DamageCue;
		DamageCue.Location = Actor->GetActorLocation();
		DamageCue.EffectSound = ImpactSound;
		if (Actor->Implements<UCombatInterface>())
		{
			/* GameplayAbility only replicated on "Owning Client" and/or "Server"
			 * NiagaraEffect Spawned here WON'T show up on others so we use GameplayCue
			 */
			DamageCue.NiagaraSystem = ICombatInterface::Execute_GetBloodEffect(Actor);
		}
		DamageCueList.DamageCues.Add(DamageCue);
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
			for(const auto& [EffectSound, NiagaraSystem, Location]
				: DamageCueList->DamageCues)
			{
				UGameplayStatics::PlaySoundAtLocation(WorldContextObject, EffectSound, Location);
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, NiagaraSystem, Location);
			}
		}
	}
}
