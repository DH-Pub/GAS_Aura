// Copyright Hung


#include "UI/Widget/GlobeWidget.h"

#include "Components/Image.h"
#include "Components/OverlaySlot.h"

void UGlobeWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	Image_Background->SetBrush(DefaultBackground);
	Image_Ring->SetBrush(RingBrush);
	
	Cast<UOverlaySlot>(Image_Background->Slot)->SetPadding(InPadding);
	Cast<UOverlaySlot>(Image_Glass->Slot)->SetPadding(InPadding);
}
