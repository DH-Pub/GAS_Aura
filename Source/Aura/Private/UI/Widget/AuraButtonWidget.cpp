// Copyright Hung


#include "UI/Widget/AuraButtonWidget.h"

#include "Components/Button.h"
#include "Components/SizeBox.h"

void UAuraButtonWidget::ButtonWidgetClicked_Implementation()
{
	Button->OnClicked.Broadcast();
}

void UAuraButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SizeBox_Root->SetWidthOverride(ButtonWidthHeight.X);
	SizeBox_Root->SetHeightOverride(ButtonWidthHeight.Y);

	Button->SetStyle(InnerStyle);
}
