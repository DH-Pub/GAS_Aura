// Copyright Hung


#include "UI/HUD/AuraHUD.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraInputAbility.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

UOverlayWidgetController* AAuraHUD::CreateOrGetOverlayWC(const FWidgetControllerParams& WCParams)
{
	return UAuraWidgetController::CreateOrGetWidgetController<UOverlayWidgetController>(
		this, OverlayWidgetController, OverlayWidgetControllerClass, WCParams);
}
UAttributeMenuWidgetController* AAuraHUD::CreateOrGetAttributeMenuWC(const FWidgetControllerParams& WCParams)
{
	return UAuraWidgetController::CreateOrGetWidgetController<UAttributeMenuWidgetController>(
		this, AttributeMenuWidgetController, AttributeMenuWidgetControllerClass, WCParams);
}
USpellMenuWidgetController* AAuraHUD::CreateOrGetSpellMenuWC(const FWidgetControllerParams& WCParams)
{
	return UAuraWidgetController::CreateOrGetWidgetController<USpellMenuWidgetController>(
		this, SpellMenuWidgetController, SpellMenuWidgetControllerClass, WCParams);
}

void AAuraHUD::InitAuraHUD(const FWidgetControllerParams& WCParams)
{
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AbilitySystemComponent->OnGiveAbilityDelegate.AddUObject(this, &AAuraHUD::BroadcastGivenAbility);
	InitOverlay(WCParams);
}
void AAuraHUD::BroadcastAllActivatableAbilities()
{
	FScopedAbilityListLock AbilityListLock(*AbilitySystemComponent);
	for (const auto& AbilitySpec : AbilitySystemComponent->GetActivatableAbilities())
	{
		BroadcastGivenAbility(AbilitySpec);
	}
}
void AAuraHUD::BroadcastGivenAbility(const FGameplayAbilitySpec& Spec)
{
	if (Spec.Ability->GetAssetTags().HasTag(AuraGameplayTags::Ability))
	{
		if (FAuraAbilityData* Data = AbilityData->FindAbilityDataByTags(Spec.Ability->GetAssetTags()))
		{
			Data->InputTag = Cast<UAuraInputAbility>(Spec.Ability)->StartupInputTag;
			Data->StatusTag = AbilitySystemComponent->GetStatusFromSpec(Spec);
			AbilityDataDelegate.Broadcast(*Data);
		}
	}
}

void AAuraHUD::InitOverlay(const FWidgetControllerParams& WCParams)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out in BP_AuraHUD"));

	// Create and add HUD widget to viewport
	OverlayWidget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget->AddToViewport();

	 /**
	  * Create/Get WidgetController, BindCallbacksDependencies()
	  */
	OverlayWidget->SetWidgetController(CreateOrGetOverlayWC(WCParams));
	OverlayWidgetController->BroadcastInitialValues();
}
