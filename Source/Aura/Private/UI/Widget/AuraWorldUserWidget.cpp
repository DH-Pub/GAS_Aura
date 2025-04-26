// Copyright Hung


#include "UI/Widget/AuraWorldUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"

void UAuraWorldUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// Assumes we are directly added to a CanvasPanel rather than direct to viewport.
	if (UCanvasPanelSlot* CanvasSlot = static_cast<UCanvasPanelSlot*>(Slot))
	{
		CanvasSlot->SetAlignment(FVector2D(0.5, 1.0));
		CanvasSlot->SetAutoSize(true);
	}

	if (!IsValid(AttachedActor)) RemoveFromParent();
}

void UAuraWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();
		return;
	}
	
	if (bFollow)
	{
		SetWorldToScreenTranslation(AttachedActor->GetActorLocation());
	}
	else
	{
		SetWorldToScreenTranslation(InitialLocation);
	}
}

void UAuraWorldUserWidget::SetWorldToScreenTranslation(const FVector& WorldLocation)
{
	FVector2D ScreenPosition;
	const bool bIsOnScreen = UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), WorldLocation + WorldOffset, ScreenPosition);
	if (bIsOnScreen) // check if widget is not outside camera view
	{
		const float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		ScreenPosition /= Scale;
		ScreenPosition.X -= GetDesiredSize().X * 0.5f;
		SetRenderTranslation(ScreenPosition);
	}
	// Avoid unnecessary invalidation is Slate
	if (bWasOnScreen != bIsOnScreen)
	{
		SetVisibility(bIsOnScreen ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		bWasOnScreen = bIsOnScreen;
	}
}
