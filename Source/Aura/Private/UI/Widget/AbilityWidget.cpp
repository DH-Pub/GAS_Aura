// Copyright Hung


#include "UI/Widget/AbilityWidget.h"

#include "UI/WidgetController/AuraWidgetController.h"

void UAbilityWidget::SetWidgetController(UAuraWidgetController* InWidgetController)
{
	Super::SetWidgetController(InWidgetController);
	WidgetController->AbilitySystemComponent->AbilityActivatedCallbacks.AddUObject(this, &UAbilityWidget::OnAbilityCommited);
}
