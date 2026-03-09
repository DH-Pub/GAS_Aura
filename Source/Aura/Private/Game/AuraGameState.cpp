// Copyright Hung


#include "Game/AuraGameState.h"

#include "EngineUtils.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interface/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AAuraGameState::AAuraGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("AuraASC"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	ServerFPS = 0.f;
}
AAuraGameState* AAuraGameState::Get(const UObject* WorldContextObject)
{
	return static_cast<AAuraGameState*>(UGameplayStatics::GetGameState(WorldContextObject));
}

UAbilitySystemComponent* AAuraGameState::GetAbilitySystemComponent() const {return AbilitySystemComponent;}

void AAuraGameState::GetLivingCharactersWithTag(const UObject* WorldContextObject, const FName Tag, TArray<AActor*>& OutActors)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (const AAuraGameState* GS = static_cast<AAuraGameState*>(World->GetGameState()))
		{
			for (AActor* Actor : GS->LivingCharacters) if (Actor->ActorHasTag(Tag)) OutActors.Add(Actor);
		}
	}
}

extern ENGINE_API float GAverageFPS;
void AAuraGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetLocalRole() == ROLE_Authority)
	{
		ServerFPS = GAverageFPS;
	}
	CheckLivingCharacters();
}

void AAuraGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AAuraGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ServerFPS);
}

void AAuraGameState::CheckLivingCharacters()
{
	LivingCharacters.Reset();
	if (const UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Actor))
			{
				LivingCharacters.Add(Actor);
			}
		}
	}
}
