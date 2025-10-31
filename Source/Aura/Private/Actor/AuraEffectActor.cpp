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
		const FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(EffectType.GameplayEffectClass, ActorLevel,
			FGameplayEffectContextHandle());
		FGameplayEffectContext* Context = SpecHandle.Data->GetContext().Get();
		Context->SetEffectCauser(this);
		FAuraEffectContext::SetIsShowDamageOnTarget(Context, true);
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

		if (bDestroyOnEffectApplication) Destroy();
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
