// Copyright Hung


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, const FEffectType& EffectType, const FHitResult& SweepResult)
{
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
	{
		check(EffectType.GameplayEffectClass);
		const FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(EffectType.GameplayEffectClass,
			ActorLevel, FGameplayEffectContextHandle());
		FGameplayEffectContext* EffectContext = SpecHandle.Data->GetContext().Get();
		EffectContext->SetEffectCauser(this);
		EffectContext->AddHitResult(SweepResult);
		// FAuraEffectContext* AuraContext = FAuraEffectContext::ExtractAuraContext(SpecHandle.Data->GetContext());
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

		if (bDestroyOnEffectApplication) Destroy();
	}
}


void AAuraEffectActor::OnOverlap(AActor* TargetActor, const FHitResult& SweepResult)
{
	if (IsNotForEnemy(TargetActor)) return;

	for (const FEffectType& Effect : Effects)
	{
		switch (Effect.ApplicationPolicy)
		{
		case EEffectApplicationPolicy::ApplyOnOverlap:
		case EEffectApplicationPolicy::ApplyOnOverlapAndRemoveOnEnd:
			ApplyEffectToTarget(TargetActor, Effect, SweepResult);
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
				if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::
					GetAbilitySystemComponentFromActor(TargetActor))
				{
					TargetASC->RemoveActiveGameplayEffectBySourceEffect(Effect.GameplayEffectClass, TargetASC, 1);
				}
			}
			break;
		case EEffectApplicationPolicy::ApplyOnEndOverlap:
			ApplyEffectToTarget(TargetActor, Effect, FHitResult());
			break;
		default:
			break;
		}
	}
}
