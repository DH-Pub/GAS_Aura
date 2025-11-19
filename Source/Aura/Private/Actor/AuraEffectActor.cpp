// Copyright Hung


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AuraEffectTypes.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, const FEffectType& EffectType)
{
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
	{
		check(EffectType.GameplayEffectClass);
		const FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(EffectType.GameplayEffectClass, ActorLevel,
			FGameplayEffectContextHandle());
		FAuraEffectContext* AuraContext = FAuraEffectContext::ExtractAuraContext(SpecHandle.Data->GetContext());
		AuraContext->SetEffectCauser(this);
		AuraContext->SetShowDamageOnTarget(true);
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

		if (bDestroyOnEffectApplication) Destroy();
	}
}


void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (IsNotForEnemy(TargetActor)) return;

	for (const FEffectType& Effect : Effects)
	{
		switch (Effect.ApplicationPolicy)
		{
		case EEffectApplicationPolicy::ApplyOnOverlap:
		case EEffectApplicationPolicy::ApplyOnOverlapAndRemoveOnEnd:
			ApplyEffectToTarget(TargetActor, Effect);
		default: break;
		}
	}
}
void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (IsNotForEnemy(TargetActor)) return;

	for (const FEffectType& Effect : Effects)
	{
		switch (Effect.ApplicationPolicy)
		{
		case EEffectApplicationPolicy::ApplyOnOverlapAndRemoveOnEnd:
			if (Effect.GameplayEffectClass.GetDefaultObject()->DurationPolicy == EGameplayEffectDurationType::Infinite)
			{
				if (UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::
					GetAbilitySystemComponentFromActor(TargetActor))
				{
					TargetAbilitySystem->RemoveActiveGameplayEffectBySourceEffect(Effect.GameplayEffectClass,
						TargetAbilitySystem, 1);
				}
			}
			break;
		case EEffectApplicationPolicy::ApplyOnEndOverlap:
			ApplyEffectToTarget(TargetActor, Effect);
			break;
		default:
			break;
		}
	}
}
