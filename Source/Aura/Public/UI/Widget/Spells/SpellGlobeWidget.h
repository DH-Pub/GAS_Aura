// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GlobeWidgetInput.h"
#include "SpellGlobeWidget.generated.h"

class UImage;
/**
 * Spell globes in-game HUD, with cooldown
 */
UCLASS()
class AURA_API USpellGlobeWidget : public UGlobeWidgetInput
{
	GENERATED_BODY()
public:
	virtual void SetWidgetController(UAuraWidgetController* InWidgetController) override;

	UPROPERTY(BlueprintReadWrite)
	FSlateBrush SpellIconBrush;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UOverlayWidgetController> OverlayWC;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<class UProgressBar> Progress_Cooldown;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_SpellIcon; // Icon of spell

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_WheelProgress;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UMaterialInstanceDynamic> WheelMaterialInstance; // Material of Image_WheelProgress
	const FName WheelPercentParam = FName("Percentage");

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<class UTextBlock> Text_Cooldown; // Cooldown Remaining


	UFUNCTION(BlueprintCallable)
	void UpdateAbilityUI();
	void CheckAbilityCooldown();
	FGameplayTagContainer CooldownTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aura|Properties")
	float Frequency = 0.05f;

	UPROPERTY(BlueprintReadWrite)
	float CooldownDuration = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float TimeRemaining = 0.f;
	UFUNCTION(BlueprintCallable)
	void EndCooldown();

	UPROPERTY(BlueprintReadWrite, Category="Properties")
	FTimerHandle CooldownTimerHandle;

	virtual void ClearGlobe() override;
private:
	UFUNCTION()
	void UpdateByTimerHandle();
};
