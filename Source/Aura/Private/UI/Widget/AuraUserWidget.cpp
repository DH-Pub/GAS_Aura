// Copyright Hung


#include "UI/Widget/AuraUserWidget.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "Kismet/KismetMathLibrary.h"

FVector2D UAuraUserWidget::GetOffsetToPivot(const FGeometry& MyGeometry, const FVector2D Offset) const
{
	const FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	FVector2D WidgetPosToTopLeft; // Vector From Widget to TopLeft of Screen
	USlateBlueprintLibrary::ScreenToWidgetLocal(this, MyGeometry, FVector2D(),
		WidgetPosToTopLeft); // LocalCoord = ScreenPos - WidgetPos
	const FVector2D MousePosOnWidget = MousePos + WidgetPosToTopLeft; // MousePos - WidgetPosToTopLeft * -1;
	const FVector2D WidgetSize = GetDesiredSize();

	const FVector2D MousePercentOnWidget(UKismetMathLibrary::SafeDivide(MousePosOnWidget.X, WidgetSize.X),
		UKismetMathLibrary::SafeDivide(MousePosOnWidget.Y, WidgetSize.Y));
	return MousePercentOnWidget - FVector2D(Offset.X, Offset.Y);
}

/*static*/ void UAuraUserWidget::SetSizeBoxWidthHeight(class USizeBox* InSizeBox, const FVector2D& InWidthHeight)
{
	InSizeBox->SetWidthOverride(InWidthHeight.X);
	InSizeBox->SetHeightOverride(InWidthHeight.Y);
}
