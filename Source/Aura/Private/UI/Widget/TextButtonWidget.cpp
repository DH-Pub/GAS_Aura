// Copyright Hung


#include "UI/Widget/TextButtonWidget.h"

#include "Components/Border.h"
#include "Components/ButtonSlot.h"
#include "Components/ScaleBox.h"
#include "Components/TextBlock.h"

void UTextButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	Border->SetBrush(BorderBrush);
	
	const float Padding_LR = ButtonWidthHeight.X * ScaleBoxPadding.X; // 6/40
	const float Padding_UD = ButtonWidthHeight.Y * ScaleBoxPadding.Y;
	Cast<UButtonSlot>(ScaleBox->Slot)->SetPadding(FMargin(Padding_LR, Padding_UD, Padding_LR, Padding_UD));
	
	Text->SetText(ButtonText);
	Text->SetFont(FontInfo);
	
	SetIsFocusable(true);
}
