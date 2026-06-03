// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GlobeWidgetInput.h"
#include "MenuEquipSpellWidget.generated.h"

/**
 * Spell globes in spell menu that can drag/drop into
 */
UCLASS()
class AURA_API UMenuEquipSpellWidget : public UGlobeWidgetInput
{
	GENERATED_BODY()
public:
	virtual void SetWidgetController(UAuraWidgetController* InWidgetController) override;
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable)
	void UpdateAbilityUI(); // Can be used to reset Globe to AbilityID

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_SpellIcon;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class USpellMenuWidgetController> SpellMenuWC;
};
