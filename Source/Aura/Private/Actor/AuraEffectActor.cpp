// Copyright Hung


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}


void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, const FEffectType& EffectType, const FHitResult& HitResult)
{
	if (IsNotForEnemy(TargetActor)) return;
	if (UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		check(EffectType.GameplayEffectClass);
		FGameplayEffectContextHandle EffectContextHandle = TargetAbilitySystem->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		UAuraAbilitySystemLibrary::SetIsShowDamageOnTarget(EffectContextHandle, true);
		const FGameplayEffectSpecHandle EffectSpecHandle = TargetAbilitySystem->MakeOutgoingSpec(EffectType.GameplayEffectClass, ActorLevel, EffectContextHandle);
		EffectSpecHandle.Data->GetContext().AddHitResult(HitResult);
		const FActiveGameplayEffectHandle ActiveEffectHandle = TargetAbilitySystem->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);
		
		// if (EffectSpecHandle.Data->Def->DurationPolicy == EGameplayEffectDurationType::Infinite
		// 	&& InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
		// {ActiveEffectHandles.Add(ActiveEffectHandle, TargetAbilitySystem);}
		// EGameplayEffectDurationType EffectDurationType = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy;
		if (bDestroyOnEffectApplication)
		{
			Destroy();
		}
	}
}


void AAuraEffectActor::OnOverlap(AActor* TargetActor, const FHitResult HitResult)
{
	if (IsNotForEnemy(TargetActor)) return;

	if (!HasAuthority()) return;
	for (const FEffectType& Effect : Effects)
	{
		if (Effect.ApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			ApplyEffectToTarget(TargetActor, Effect, HitResult);
		}
	}
}
void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (IsNotForEnemy(TargetActor)) return;

	if (!HasAuthority()) return;
	for (const FEffectType& Effect : Effects)
	{
		if (Effect.ApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			FHitResult HitResult;
			ApplyEffectToTarget(TargetActor, Effect, HitResult);
		}

		if (Effect.GameplayEffectClass.GetDefaultObject()->DurationPolicy == EGameplayEffectDurationType::Infinite
			&& Effect.RemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
		{
			UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
			if (!IsValid(TargetAbilitySystem)) return;
		
			TargetAbilitySystem->RemoveActiveGameplayEffectBySourceEffect(Effect.GameplayEffectClass, TargetAbilitySystem, 1);
		}
	}
}
