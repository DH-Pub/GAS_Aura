// Copyright Hung


#include "UI/HUD/AuraHUD.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraLibrary.h"
#include "AbilitySystem/Abilities/CostCooldownAbility.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

UOverlayWidgetController* AAuraHUD::CreateOrGetOverlayWC()
{
	if (OverlayWidgetController) return OverlayWidgetController;
	return UAuraWidgetController::CreateOrGetWidgetController<UOverlayWidgetController>(
		this, OverlayWidgetController, OverlayWidgetControllerClass, GetHUDControllerParams());
}
UAttributeMenuWidgetController* AAuraHUD::CreateOrGetAttributeMenuWC()
{
	if (AttributeMenuWidgetController) return AttributeMenuWidgetController;
	return UAuraWidgetController::CreateOrGetWidgetController<UAttributeMenuWidgetController>(
		this, AttributeMenuWidgetController, AttributeMenuWidgetControllerClass, GetHUDControllerParams());
}
USpellMenuWidgetController* AAuraHUD::CreateOrGetSpellMenuWC()
{
	if (SpellMenuWidgetController) return SpellMenuWidgetController;
	return UAuraWidgetController::CreateOrGetWidgetController<USpellMenuWidgetController>(
		this, SpellMenuWidgetController, SpellMenuWidgetControllerClass, GetHUDControllerParams());
}

void AAuraHUD::InitAuraHUD(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
	InitOverlay();
}
void AAuraHUD::BroadcastAllActivatableAbilities() const
{
	FScopedAbilityListLock AbilityListLock(*AbilitySystemComponent);
	for (const FGameplayAbilitySpec& AbilitySpec : AbilitySystemComponent->GetActivatableAbilities())
	{
		AbilitySystemComponent->ClientUpdateAbilityData(AbilitySpec);
	}
}

void AAuraHUD::InitOverlay()
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out in BP_AuraHUD"));

	// Create and add HUD widget to viewport
	OverlayWidget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget->AddToViewport();

	/**
	 * Create/Get WidgetController, BindCallbacksDependencies()
	 */
	OverlayWidget->SetWidgetController(CreateOrGetOverlayWC());
	OverlayWidgetController->BroadcastInitialValues();
}

FWidgetControllerParams AAuraHUD::GetHUDControllerParams() const
{
	return FWidgetControllerParams(PlayerController, PlayerState, AbilitySystemComponent, AttributeSet);
}
