// Copyright Hung


#include "UI/HUD/AuraHUD.h"

#include "AbilitySystem/Data/AttributeDataAsset.h"
#include "Components/Overlay.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

void AAuraHUD::InitAuraHUD(class UAuraAbilitySystemComponent* InASC)
{
	UAuraWidgetController::CreateOrGetWidgetController<UOverlayWidgetController>(OverlayController, InASC);
	OverlayWidget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget->AddToViewport();
	OverlayWidget->SetWidgetController(OverlayController);

	UAuraWidgetController::CreateOrGetWidgetController<UAttributeMenuWidgetController>(AttributeMenuController, InASC);
	UAuraWidgetController::CreateOrGetWidgetController<USpellMenuWidgetController>(SpellMenuController, InASC);
}

const TMap<FGameplayTag, FAuraAttributeData>& AAuraHUD::GetAttributeDataList() const
{return AttributeData->AttributeDataList;}


#pragma region UIFunctions
AAuraHUD* AAuraHUD::Get(const UObject* WorldContextObject)
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
	const AAuraHUD* HUD = Get(InNewWidget);
	if (HUD && HUD->OverlayController && HUD->OverlayController->Overlay_Screen)
	{	// Set inside WBP_Overlay->BP_SetWidgetController
		/*UOverlaySlot* Slot =*/ HUD->OverlayController->Overlay_Screen->AddChildToOverlay(InNewWidget);
		return true;
	}
	return false;
}
#pragma endregion
