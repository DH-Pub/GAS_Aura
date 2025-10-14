// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GlobeWidget.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "SpellGlobeWidget.generated.h"

class UProgressBar;
struct FAuraAbilityData;
struct FGameplayTag;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class AURA_API USpellGlobeWidget : public UGlobeWidget
{
	GENERATED_BODY()
public:
	virtual void SetWidgetController(UAuraWidgetController* InWidgetController) override;
	
	UPROPERTY(BlueprintReadWrite, Category = "Default|GlobeProperties")
	FSlateBrush SpellIconBrush;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|GlobeProperties", meta=(GameplayTagFilter=Input))
	FGameplayTag SlotTag; // Player input for spells, set in overlay first, DO NOT modify this inside C++
	FGameplayTag AbilityTag; // Set on InputTag Matches, Remove this and everything else when changed to other Slot
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
	bool SuccessUpdateAbilityData(const FAuraAbilityData& InAbilityData, const FPlayerAbilityData& InPlayerData,
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
