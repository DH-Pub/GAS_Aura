// Copyright Hung


#include "UI/Widget/AuraUserWidget.h"

void UAuraUserWidget::SetWidgetController(UAuraWidgetController* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet(); // Call event in blueprint
}
