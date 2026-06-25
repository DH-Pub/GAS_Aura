// Copyright Hung


#include "UI/Widget/AuraPrimaryLayout.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer, "UI.Layer", "")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_Game, "UI.Layer.Game", "In game HUD")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_GameMenu, "UI.Layer.GameMenu", "Inventory, ...")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_Menu, "UI.Layer.Menu", "Settings Screen")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_Menu_Sub, "UI.Layer.Menu.Sub", "Sub-Menu")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_Modal, "UI.Layer.Modal", "Confirmation Dialog, Error Dialog, ...")


UAuraActivatableWidget* UAuraPrimaryLayout::PushWidgetToLayer(const EUI_Layer InLayer,
	const TSubclassOf<UAuraActivatableWidget> ActivatableWidgetClass, const float InTransitionDuration,
	const bool bClearLayer)
{
	if (InLayer != None)
	{
		if (UCommonActivatableWidgetContainerBase* Layer = StackLayers.FindRef(InLayer))
		{
			if (InTransitionDuration >= 0.f) Layer->SetTransitionDuration(InTransitionDuration);
			if (bClearLayer) Layer->ClearWidgets();
			// Layer->GetWidgetList()

			UAuraActivatableWidget* ActivatableWidget = Layer->AddWidget<UAuraActivatableWidget>(ActivatableWidgetClass);
			ActivatableWidget->OverallLayout = this;
			return ActivatableWidget;
		}
	}
	return nullptr;
}

void UAuraPrimaryLayout::FindAndRemoveActivatableWidget(UCommonActivatableWidget* ActivatableWidget,
	const float InTransitionDuration)
{
	if (!ActivatableWidget) return;
	for (const auto& [LayerEnum, Layer] : StackLayers)
	{
		if (InTransitionDuration >= 0.f) Layer->SetTransitionDuration(InTransitionDuration);

		Layer->RemoveWidget(*ActivatableWidget);
	}
}
