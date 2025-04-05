// Copyright Hung


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}


void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, const FEffectType& EffectType)
{
	if (UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		check(EffectType.GameplayEffectClass);
		FGameplayEffectContextHandle EffectContextHandle = TargetAbilitySystem->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		const FGameplayEffectSpecHandle EffectSpecHandle = TargetAbilitySystem->MakeOutgoingSpec(EffectType.GameplayEffectClass, ActorLevel, EffectContextHandle);
		const FActiveGameplayEffectHandle ActiveEffectHandle = TargetAbilitySystem->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);
	
		// if (EffectSpecHandle.Data->Def->DurationPolicy == EGameplayEffectDurationType::Infinite
		// 	&& InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
		// {
		// 	ActiveEffectHandles.Add(ActiveEffectHandle, TargetAbilitySystem);
		// }
	}
}


void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
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
	for (const FEffectType& Effect : Effects)
	{
		if (Effect.ApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectToTarget(TargetActor, Effect);
		}

		if (Effect.GameplayEffectClass.GetDefaultObject()->DurationPolicy == EGameplayEffectDurationType::Infinite
			&& Effect.RemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
		{
			UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
			if (!IsValid(TargetAbilitySystem)) return;
		
			TargetAbilitySystem->RemoveActiveGameplayEffectBySourceEffect(Effect.GameplayEffectClass, TargetAbilitySystem, 1);

			// FGameplayEffectQuery EffectQuery;
			// EffectQuery.EffectSource = this;
			// TArray<FActiveGameplayEffectHandle> ActiveEffects = TargetAbilitySystem->GetActiveEffects(EffectQuery);

		}
	}
}
