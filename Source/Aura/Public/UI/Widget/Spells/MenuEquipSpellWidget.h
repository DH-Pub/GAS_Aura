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

	virtual void ClearGlobe() override;

	UFUNCTION()
	void ReceiveAbilityData(const struct FGameplayAbilitySpec& AbilitySpec, const struct FAuraAbilityData& Data);

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_SpellIcon;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class USpellMenuWidgetController> SpellMenuWC;

	UPROPERTY(BlueprintReadWrite)
	FSlateBrush CurrentIcon = FSlateBrush(); // Save Ability Icon to this
	UPROPERTY(BlueprintReadWrite)
	FSlateBrush CurrentBackground = FSlateBrush(); // Save Ability Background to this
};
