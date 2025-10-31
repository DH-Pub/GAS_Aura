// Copyright Hung


#include "Game/AuraGameMode.h"

#include "Player/AuraPlayerController.h"

void AAuraGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(NewPlayer)) PlayerControllers.Add(PC);
}
void AAuraGameMode::Logout(AController* Exiting)
{
	if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Exiting)) PlayerControllers.Remove(PC);
	Super::Logout(Exiting);
}
