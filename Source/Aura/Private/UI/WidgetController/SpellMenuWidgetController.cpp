// Copyright Hung


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void USpellMenuWidgetController::BindCallbacksDependencies()
{
	OverlayWC = UAuraAbilitySystemLibrary::GetOverlayWidgetController(this);
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	for (const auto& AbilitySpec : AbilitySystemComponent->GetActivatableAbilities())
	{
		OverlayWC->BroadcastGivenAbility(AbilitySpec);
	}
}

UOverlayWidgetController* USpellMenuWidgetController::GetOverlayWidgetController()
{
	if (OverlayWC == nullptr)
	{
		OverlayWC = UAuraAbilitySystemLibrary::GetOverlayWidgetController(this);
	}
	return OverlayWC;
}
