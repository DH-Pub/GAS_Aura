// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraUserWidget.h"
#include "AbilityWidget.generated.h"

class UGameplayAbility;
class UAuraWidgetController;
/**
 * 
 */
UCLASS()
class AURA_API UAbilityWidget : public UAuraUserWidget
{
	GENERATED_BODY()
public:
	virtual void SetWidgetController(UAuraWidgetController* InWidgetController) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnAbilityCommited(UGameplayAbility* Ability);
};
