// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GlobeWidgetInput.h"
#include "SpellGlobeWidget.generated.h"

enum class EAuraAbilityPassiveID : uint8;
enum class EAuraAbilityInputID : uint8;
class UProgressBar;
class UTextBlock;
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
	TObjectPtr<UProgressBar> Progress_Cooldown;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_SpellIcon; // Icon of spell

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_WheelProgress;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UMaterialInstanceDynamic> WheelMaterialInstance; // Material of Image_WheelProgress
	const FName WheelPercentParam = FName("Percentage");

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Cooldown; // Cooldown Remaining


	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs = "ReturnValue"))
	bool SuccessUpdateAbilityData(const struct FAuraAbilityData& InAbilityData, const struct FPlayerAbilityData& InPlayerData,
		FGameplayTagContainer& OutCooldownTags);

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UAsync_CooldownChange> WaitCDTask;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Default|Properties")
	float Frequency = 0.05f;

	bool bOnCooldown = false;
	UPROPERTY(BlueprintReadWrite)
	float CooldownDuration = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float TimeRemaining = 0.f;
	UFUNCTION(BlueprintCallable)
	void UpdateCooldown(float InTime, float InDuration = -1.f);
	UFUNCTION(BlueprintCallable)
	void EndCooldown();

	UPROPERTY(BlueprintReadWrite, Category="Properties")
	FTimerHandle CooldownTimerHandle;
private:
	UFUNCTION()
	void UpdateByTimerHandle();
	virtual void ClearGlobe();
};
