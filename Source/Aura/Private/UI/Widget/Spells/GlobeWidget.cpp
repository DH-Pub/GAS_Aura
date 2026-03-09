// Copyright Hung


#include "UI/Widget/Spells/GlobeWidget.h"

#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"

void UGlobeWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (SizeBox_Root)
	{
		SizeBox_Root->SetWidthOverride(WidthHeight.X);
		SizeBox_Root->SetHeightOverride(WidthHeight.Y);
		SizeBox_Root->SetMinAspectRatio(1.f);
		SizeBox_Root->SetMaxAspectRatio(1.f);
	}

	Image_Background->SetBrush(DefaultBackground);
	Cast<UOverlaySlot>(Image_Background->Slot)->SetPadding(InPadding);
	if (Image_Ring) Image_Ring->SetBrush(RingBrush);
	if (Image_Glass) Cast<UOverlaySlot>(Image_Glass->Slot)->SetPadding(InPadding);
}
