// Copyright Hung


#include "Game/AuraGameModeBase.h"

#include "Player/AuraPlayerController.h"

void AAuraGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(NewPlayer)) PlayerControllers.Add(PC);
}
void AAuraGameModeBase::Logout(AController* Exiting)
{
	if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Exiting)) PlayerControllers.Remove(PC);
	Super::Logout(Exiting);
}
