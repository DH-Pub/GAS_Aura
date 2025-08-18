// Copyright Hung


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Player/AuraPlayerState.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void USpellMenuWidgetController::BindCallbacksDependencies()
{
	OverlayWC = UAuraAbilitySystemLibrary::GetOverlayWidgetController(this);
	PlayerState->OnSpellPointsChangedDelegate.AddLambda([this](const int32 Points)
	{
		SpellPointsToUIDelegate.Broadcast(Points);
	});
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	for (const auto& AbilitySpec : AbilitySystemComponent->GetActivatableAbilities())
	{
		OverlayWC->BroadcastGivenAbility(AbilitySpec);
	}
	SpellPointsToUIDelegate.Broadcast(PlayerState->GetSpellPoints());
}

UOverlayWidgetController* USpellMenuWidgetController::GetOverlayWidgetController()
{
	if (OverlayWC == nullptr)
	{
		OverlayWC = UAuraAbilitySystemLibrary::GetOverlayWidgetController(this);
	}
	return OverlayWC;
}
