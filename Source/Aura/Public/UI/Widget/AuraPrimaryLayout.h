// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "PrimaryGameLayout.h"
#include "CommonActivatableWidget.h"

#include "AuraPrimaryLayout.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer); // Essentially Enum for UPrimaryGameLayout
UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Game);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_GameMenu);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Menu);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Menu_Sub);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Modal);

UENUM(BlueprintType)
enum EUI_Layer : uint8
{
	None,
	Game, // In game HUD
	GameMenu, // Game "Menu", like Inventory, ...
	Menu, // Settings Screen
	SubMenu, // Sub-Menu
	Modal // Confirmation Dialog, Error Dialog, ...
};

class UAuraActivatableWidget;
/**
 * TODO: UPrimaryGameLayout may not be needed and just inherit from UCommonUserWidget instead
 */
UCLASS()
class AURA_API UAuraPrimaryLayout : public UPrimaryGameLayout
{
	GENERATED_BODY()
public:
	/**
	 * Override this because UCommonActivatableWidgetContainerBase::BP_AddWidget (Push Widget) is private
	 * @param InLayer
	 * @param ActivatableWidgetClass
	 * @param bClearLayer Layer Clear Widgets on push
	 * @return DeterminesOutputType=ActivatableWidgetClass
	 */
	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=ActivatableWidgetClass))
	UAuraActivatableWidget* PushWidgetToLayer(const EUI_Layer InLayer,
		const TSubclassOf<UAuraActivatableWidget> ActivatableWidgetClass, const float InTransitionDuration = -1.f,
		const bool bClearLayer = false);

	UFUNCTION(BlueprintCallable)
	void FindAndRemoveActivatableWidget(UCommonActivatableWidget* ActivatableWidget, const float InTransitionDuration = -1.f);

protected:
	UPROPERTY(BlueprintReadWrite, meta=(GameplayTagFilter="UI.Layer"))
	TMap<TEnumAsByte<EUI_Layer>, TObjectPtr<UCommonActivatableWidgetContainerBase>> StackLayers;
};

/**
 *
 */
UCLASS()
class AURA_API UAuraActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=ActivatableWidgetClass))
	UAuraActivatableWidget* PushWidgetToPrimaryLayout(const EUI_Layer Layer,
		const TSubclassOf<UAuraActivatableWidget> ActivatableWidgetClass, const float InTransitionDuration = -1.f,
		const bool bClearOtherWidgets = false)
	{
		return OverallLayout->PushWidgetToLayer(Layer, ActivatableWidgetClass, InTransitionDuration, bClearOtherWidgets);
	};

	UFUNCTION(BlueprintCallable, Category = ActivatableWidget)
	void DeactivateWithTransition(const float InTransitionDuration = -1.f)
	{
		OverallLayout->FindAndRemoveActivatableWidget(this, InTransitionDuration);
	};

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UAuraPrimaryLayout> OverallLayout;
};
