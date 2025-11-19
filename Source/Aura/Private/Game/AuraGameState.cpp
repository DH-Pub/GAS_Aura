// Copyright Hung


#include "Game/AuraGameState.h"

#include "EngineUtils.h"
#include "Interface/CombatInterface.h"

AAuraGameState::AAuraGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

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

void AAuraGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckLivingCharacters();
}

void AAuraGameState::CheckLivingCharacters()
{
	LivingCharacters.Reset();
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
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
