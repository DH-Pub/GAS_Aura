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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|GlobeProperties")
	FSlateBrush SpellIconBrush;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|GlobeProperties")
	FSlateBrush WheelBrush; // Cooldown Wheel
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|GlobeProperties", meta=(GameplayTagFilter=Input))
	FGameplayTag InputTag; // Player input for spells, set in overlay first
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
	
	virtual void WidgetControllerSet_Implementation(UAuraWidgetController* Controller) override;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UOverlayWidgetController> OverlayWC;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UProgressBar> Progress_Cooldown;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_SpellIcon; // Icon of spell
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_WheelProgress;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UMaterialInstanceDynamic> WheelMaterialInstance;
	const FName WheelPercentParam = FName("Percentage");
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Cooldown; // Cooldown Remaining
	
	UPROPERTY(BlueprintReadWrite, Category = "GlobeProperties", meta=(GameplayTagFilter=Cooldown))
	FGameplayTag CooldownTag;
	
	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs = "ReturnValue"))
	bool SuccessUpdateAbilityData(const FAuraAbilityData& InAbilityData);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Default|Properties")
	float Frequency = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|GlobeProperties")
	float DisableTint = 0.1f; // force value in NativePreConstruct()

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
