// Copyright Hung


#include "UI/Widget/AuraUserWidget.h"

void UAuraUserWidget::SetWidgetController(UAuraWidgetController* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet(InWidgetController); // Call event in blueprint
}

void UAuraUserWidget::WidgetControllerSet_Implementation(UAuraWidgetController* Controller)
{
	
}
