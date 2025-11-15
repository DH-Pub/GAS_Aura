// Copyright Hung


#include "UI/HUD/AuraHUD.h"

#include "AbilitySystem/Data/AttributeDataAsset.h"
#include "Character/AuraCharacterBase.h"
#include "Components/Overlay.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

void AAuraHUD::InitAuraHUD(AAuraPlayerController* PC, AAuraPlayerState* PS, AAuraCharacterBase* Character)
{
	PlayerController = PC;
	PlayerState = PS;
	AbilitySystemComponent = Character->GetAuraAbilitySystemComponent();
	AttributeSet = Character->GetAttributeSet();
	
	// Create and add HUD widget to viewport
	OverlayWidget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget->AddToViewport();
	/* Create/Get WidgetController, BindCallbacksDependencies() */
	UAuraWidgetController::CreateOrGetWidgetController<UOverlayWidgetController>(this, Character,
		OverlayController, OverlayWidgetControllerClass);
	OverlayWidget->SetWidgetController(OverlayController);
	
	UAuraWidgetController::CreateOrGetWidgetController<UAttributeMenuWidgetController>(this, Character,
		AttributeMenuController, AttributeMenuWidgetControllerClass);
	UAuraWidgetController::CreateOrGetWidgetController<USpellMenuWidgetController>(this, Character,
		SpellMenuController, SpellMenuWidgetControllerClass);
}

const TMap<FGameplayTag, FAuraAttributeData>& AAuraHUD::GetAttributeDataList() const
{return AttributeData->AttributeDataList;}


#pragma region UIFunctions
AAuraHUD* AAuraHUD::GetAuraHUD(const UObject* WorldContextObject)
{	/*TArray<APlayerController*> PlayerList; GEngine->GetAllLocalPlayerControllers(PlayerList);*/
	// UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	// WorldContextObject->GetWorld()->GetFirstPlayerController(); // ??? not consistent if server has no player
	if (const APlayerController* PC = GEngine->GetFirstLocalPlayerController(WorldContextObject->GetWorld()))
	{
		return PC->GetHUD<AAuraHUD>();
	}
	return nullptr;
}
bool AAuraHUD::AddWidgetToRootCanvasPanel(UUserWidget* InNewWidget)
{
	if (InNewWidget == nullptr) return false;
	if (const AAuraHUD* HUD = GetAuraHUD(InNewWidget);
		HUD && HUD->OverlayController && HUD->OverlayController->Overlay_Screen)
	{	// Set inside WBP_Overlay->WidgetControllerSet
		/*UOverlaySlot* Slot =*/ HUD->OverlayController->Overlay_Screen->AddChildToOverlay(InNewWidget);
		return true;
	}
	return false;
}
#pragma endregion
