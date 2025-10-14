// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

class USpellGlobeButtonWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellPointsChanged, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnSpellButtonFocused, bool, bSpendEnabled, bool, bEquipEnabled,
	const FText&, AbilityDescriptions, const FText&, LevelChangeDescriptions, bool, bSelected);

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

	UPROPERTY(BlueprintAssignable)
	FOnSpellPointsChanged SpellPointsToUIDelegate;

	UFUNCTION(BlueprintCallable)
	void ClearSelected();
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USpellGlobeButtonWidget> SelectedSpellGlobe = nullptr;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USpellGlobeButtonWidget> FocusSpellGlobe = nullptr;

	int32 SpellPoints = 0;
	void UpdateButtonsAndDescriptions(const int32 Points, const FGameplayTag& AbilityTag, const FGameplayTag& Status,
		const bool bClick = false); // Enable Buttons depending on the state of HoveredSpellGlobe
	UPROPERTY(BlueprintAssignable)
	FOnSpellButtonFocused SpellButtonFocusDelegate;

	UFUNCTION(BlueprintCallable)
	void SpendPoint();

	UFUNCTION(BlueprintCallable, meta=(ReturnDisplayName="IsPassive"))
	bool EquipAbility();
	/**
	 * @param NewSlotTag: Can be !IsValid() for when you want to clear ability from input
	 */
	UFUNCTION(BlueprintCallable)
	void ChangeSpellInputSlot(const FGameplayTag& AbilityTag, const FGameplayTag NewSlotTag = FGameplayTag(),
		const bool bIsPassive = false);
};
