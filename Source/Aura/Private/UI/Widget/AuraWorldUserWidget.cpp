// Copyright Hung


#include "UI/Widget/AuraWorldUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"

void UAuraWorldUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// Assumes we are directly added to a CanvasPanel rather than direct to viewport.
	/*if (UCanvasPanelSlot* CanvasSlot = static_cast<UCanvasPanelSlot*>(Slot))
	{
		CanvasSlot->SetAlignment(FVector2D(0.5, .5));
		CanvasSlot->SetAutoSize(true);
	}*/
	// SetAlignmentInViewport(FVector2D(1., .5));
	SetWorldToScreenTranslation(InitialLocation);
}

void UAuraWorldUserWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	if (!IsValid(AttachedActor)) {RemoveFromParent(); return;}
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bFollow) SetWorldToScreenTranslation(AttachedActor->GetActorLocation());
	else SetWorldToScreenTranslation(InitialLocation);
}

void UAuraWorldUserWidget::SetWorldToScreenTranslation(const FVector& WorldLocation)
{
	const APlayerController* PlayerController = GetOwningPlayer();
	FVector2D ScreenPosition;
	bool bIsOnScreen = UGameplayStatics::ProjectWorldToScreen(PlayerController, WorldLocation, ScreenPosition, true);
	int32 ViewportX = 0, ViewportY = 0; PlayerController->GetViewportSize(ViewportX, ViewportY);
	bIsOnScreen &= ScreenPosition.X > 0.f && ScreenPosition.X < ViewportX
		&& ScreenPosition.Y > 0.f && ScreenPosition.Y < ViewportY;
	if (bIsOnScreen) // check if widget is not outside camera view
	{
		const float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		ScreenPosition /= Scale;
		ScreenPosition -= GetDesiredSize() * .5f + ScreenOffset;
		SetRenderTranslation(ScreenPosition);
	}
	if (bWasOnScreen != bIsOnScreen)
	{	// Avoid unnecessary invalidation is Slate
		SetVisibility(bIsOnScreen ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		bWasOnScreen = bIsOnScreen;
	}
}
