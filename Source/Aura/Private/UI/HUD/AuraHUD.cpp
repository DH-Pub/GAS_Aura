// Copyright Hung


#include "UI/HUD/AuraHUD.h"

#include "AbilitySystem/Data/AttributeDataAsset.h"
#include "Components/Overlay.h"
#include "Engine/Engine.h"
#include "Framework/Application/NavigationConfig.h"
#include "Framework/Application/SlateApplication.h"
#include "UI/Widget/AuraPrimaryLayout.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

void AAuraHUD::InitAuraHUD(class UAuraAbilitySystemComponent* InASC, const bool bASCCanBeNull)
{
	if (!OverallLayout)
	{
		OverallLayout = CreateWidget<UAuraPrimaryLayout>(GetOwningPlayerController(), OverallLayoutClass);
		OverallLayout->AddToViewport();
	}

	if (InASC || bASCCanBeNull)
	{
		UAuraWidgetController::CreateOrGetWidgetController<UOverlayWidgetController>(OverlayController, InASC);
		UAuraWidgetController::CreateOrGetWidgetController<UAttributeMenuWidgetController>(AttributeMenuController, InASC);
		UAuraWidgetController::CreateOrGetWidgetController<USpellMenuWidgetController>(SpellMenuController, InASC);

		BP_OnWidgetControllersSet();
	}
}

const TMap<FGameplayTag, FAuraAttributeData>& AAuraHUD::GetAttributeDataList() const
{return AttributeData->AttributeDataList;}


#pragma region UIFunctions
AAuraHUD* AAuraHUD::Get(const UObject* WorldContextObject)
{	/*TArray<APlayerController*> PlayerList; GEngine->GetAllLocalPlayerControllers(PlayerList);*/
	// UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	// WorldContextObject->GetWorld()->GetFirstPlayerController(); // ??? not consistent if server has no player
	/*UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject); GameInstance->GetLocalPlayers();
	APlayerController* PlayerController = GameInstance->GetPrimaryPlayerController(false);
	PlayerController->GetLocalPlayer();*/
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
	{	// Set inside W_GameHUD->BP_Construct
		/*UOverlaySlot* Slot =*/ HUD->OverlayController->Overlay_Screen->AddChildToOverlay(InNewWidget);
		return true;
	}
	return false;
}

void AAuraHUD::BeginPlay()
{
	InitAuraHUD(nullptr);

	Super::BeginPlay(); // BP_BeginPlay() is called here

	TSharedRef<FNavigationConfig> NavConfig = FSlateApplication::Get().GetNavigationConfig();
	NavConfig->bTabNavigation = false; // Let Tab be bound to something
	NavConfig->bKeyNavigation = false;
	NavConfig->bAnalogNavigation = false; // Let Player move while in UI
}
#pragma endregion
