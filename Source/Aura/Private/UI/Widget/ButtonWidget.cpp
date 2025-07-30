// Copyright Hung


#include "UI/Widget/ButtonWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/ScaleBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

void UButtonWidget::ButtonWidgetClicked_Implementation()
{
	Button->OnClicked.Broadcast();
}

void UButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SizeBox_Root->SetWidthOverride(ButtonWidthHeight.X);
	SizeBox_Root->SetHeightOverride(ButtonWidthHeight.Y);

	Border->SetBrush(BorderBrush);

	Button->SetStyle(InnerStyle);

	const float Padding_LR = ButtonWidthHeight.X * 0.15; // 6/40
	const float Padding_UD = ButtonWidthHeight.Y * 0.15;
	Cast<UButtonSlot>(ScaleBox->Slot)->SetPadding(FMargin(Padding_LR, Padding_UD, Padding_LR, Padding_UD));
	
	Text->SetText(ButtonText);
	Text->SetFont(FontInfo);
}
