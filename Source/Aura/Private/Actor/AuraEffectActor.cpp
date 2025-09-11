// Copyright Hung


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraLibrary.h"

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

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, const FEffectType& EffectType)
{
	if (IsNotForEnemy(TargetActor)) return;
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		check(EffectType.GameplayEffectClass);
		FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		// EffectContextHandle.AddOrigin() // DEPRECATED: for showing dmg, but using ActorLocation
		UAuraLibrary::SetIsShowDamageOnTarget(EffectContextHandle, true);
		const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(EffectType.GameplayEffectClass, ActorLevel, EffectContextHandle);
		const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);
		
		if (bDestroyOnEffectApplication) { Destroy(); }
	}
}


void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (IsNotForEnemy(TargetActor)) return;

	if (!HasAuthority()) return;
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
	if (IsNotForEnemy(TargetActor)) return;

	if (!HasAuthority()) return;
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
		}
	}
}
