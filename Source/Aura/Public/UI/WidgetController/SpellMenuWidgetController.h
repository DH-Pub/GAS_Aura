// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

struct FAuraAbilityData;
class UAuraButtonWidget;
class URichTextBlock;
class UAbilityDataAsset;
class USpellGlobeButtonWidget;
class UOverlayWidgetController;
DECLARE_MULTICAST_DELEGATE(FSpellGlobeButtonUpdated)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellPointsChanged, int32, NewValue);
/**
 * 
 */
UCLASS()
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BindCallbacksDependencies() override;
	virtual void BroadcastInitialValues() override;
	
	UPROPERTY(BlueprintReadOnly, BlueprintGetter=GetOverlayWC)
	TObjectPtr<UOverlayWidgetController> OverlayWC;
	
	UPROPERTY(BlueprintAssignable)
	FOnSpellPointsChanged SpellPointsToUIDelegate;

	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="SpellGLobeButton"))
	void ActivateSpellGlobe(USpellGlobeButtonWidget* SpellGLobeButton, const bool bClick = false);
	UFUNCTION(BlueprintCallable)
	void ClearSelected();
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USpellGlobeButtonWidget> SelectedSpellGlobe = nullptr;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USpellGlobeButtonWidget> HoveredSpellGlobe = nullptr;

	int32 SpellPoints = 0;
	void UpdateSpendEquipButtons(const int32 Points); // Enable Buttons depending on the state of HoveredSpellGlobe

	UFUNCTION(BlueprintCallable)
	void SpendPoint();
#pragma region WidgetPointer
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAuraButtonWidget> SpendPointButton;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAuraButtonWidget> EquipButton;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<URichTextBlock> RichTextDescription;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<URichTextBlock> RichTextNextLevel;
#pragma endregion
private:
	UPROPERTY()
	TObjectPtr<UAbilityDataAsset> AbilityData;
	UFUNCTION(BlueprintGetter)
	UOverlayWidgetController* GetOverlayWC();

	UFUNCTION()
	void AbilityDataUpdated(const FAuraAbilityData& Data);
};
