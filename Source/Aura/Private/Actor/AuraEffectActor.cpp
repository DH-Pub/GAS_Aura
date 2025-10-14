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
	if (IsNotForEnemy(TargetActor)) return;
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
	{
		check(EffectType.GameplayEffectClass);
		FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		EffectContextHandle.AddOrigin(TargetActor->GetActorLocation());
		FAuraEffectContext::SetIsShowDamageOnTarget(EffectContextHandle.Get(), true);
		const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(EffectType.GameplayEffectClass, ActorLevel, EffectContextHandle);
		TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);

		if (bDestroyOnEffectApplication) { Destroy(); }
	}
}


void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (!HasAuthority() || IsNotForEnemy(TargetActor)) return;

	for (const FEffectType& Effect : Effects)
	{
		if (Effect.ApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			ApplyEffectToTarget(TargetActor, Effect);
		}
	}
}
void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (!HasAuthority() || IsNotForEnemy(TargetActor)) return;

	for (const FEffectType& Effect : Effects)
	{
		if (Effect.ApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectToTarget(TargetActor, Effect);
		}

		if (Effect.GameplayEffectClass.GetDefaultObject()->DurationPolicy == EGameplayEffectDurationType::Infinite
			&& Effect.RemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
		{
			UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
			if (!IsValid(TargetAbilitySystem)) return;

			TargetAbilitySystem->RemoveActiveGameplayEffectBySourceEffect(Effect.GameplayEffectClass, TargetAbilitySystem, 1);
		}
	}
}
