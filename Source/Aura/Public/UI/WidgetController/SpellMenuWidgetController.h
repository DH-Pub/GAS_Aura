// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Aura/Aura.h"
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
	virtual void UnbindOldAbilitySystemComponent() override;
	virtual void BindCallbacksDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category="Aura|Delegate")
	FOnReceiveAbilityDataSignature OnReceiveAbilityDataFromASC;

	UPROPERTY(BlueprintAssignable)
	FOnSpellPointsChanged SpellPointsToUIDelegate;

	UFUNCTION(BlueprintCallable)
	void ClearSelected();
	/*UPROPERTY(BlueprintReadOnly)
	TSubclassOf<UAuraGameplayAbility> SelectedClass;
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<UAuraGameplayAbility> FocusClass;*/
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USpellGlobeButtonWidget> SelectedSpellGlobe = nullptr;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USpellGlobeButtonWidget> FocusSpellGlobe = nullptr;

	int32 SpellPoints = 0;
	void UpdateButtonsAndDescriptions(const bool bClick = false) const; // Enable Buttons depending on the state of FocusSpellGlobe
	UPROPERTY(BlueprintAssignable)
	FOnSpellButtonFocused SpellButtonFocusDelegate;

	UFUNCTION(BlueprintCallable)
	void SpendPoint();

	UFUNCTION(BlueprintCallable, meta=(ReturnDisplayName="IsPassive"))
	bool EquipAbility();
	/**
	 * @param AbilityClass
	 * @param AbilityID: 0 (None) to unequip ability
	 */
	UFUNCTION(BlueprintCallable)
	void ChangeSpellInputSlot(const TSubclassOf<UGameplayAbility> AbilityClass,
		const EAuraAbilityInputID::Type AbilityID = EAuraAbilityInputID::None);
};
