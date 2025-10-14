// Copyright Hung


#include "UI/Widget/AuraUserWidget.h"

#include "UI/WidgetController/AuraWidgetController.h"

void UAuraUserWidget::SetWidgetController(UAuraWidgetController* InWidgetController)
{
	WidgetControllerSet(InWidgetController); // Call event in blueprint
}

void UAuraUserWidget::WidgetControllerSet_Implementation(UAuraWidgetController* Controller)
{
	
}
