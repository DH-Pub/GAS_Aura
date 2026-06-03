// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

USTRUCT()
struct FMessageRow : public FTableRowBase // for MessageDataTable
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta=(GameplayTagFilter="Message"))
	FGameplayTag MessageTag = FGameplayTag(); // Set RowName the same as this

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UAuraUserWidget> MessageWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(MultiLine))
	FText Message = FText();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageTableSignature, const FMessageRow&, Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXPChangedSignature, int32, Level, float, XPPercent);
/**
 *  Created in AuraHUD, which is only accessible by local client
 */
UCLASS()
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void UnbindOldAbilitySystemComponent() override;
	virtual void BindCallbacksDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category="Aura|Delegate")
	FOnReceiveAbilityDataSignature OnReceiveAbilityDataFromASC;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnVitalAttributeChanged OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnVitalAttributeChanged OnMaxHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnVitalAttributeChanged OnManaChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnVitalAttributeChanged OnMaxManaChanged;

	/**
	 * Set in WBP_Overlay
	 * For AAuraHUD::AddWidgetToRootCanvasPanel
	 */
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UOverlay> Overlay_Screen;

	UPROPERTY(BlueprintAssignable)
	FMessageTableSignature MessageTableDelegate; //TODO: DEPRECATED using GameplayCue Item Pickup Message

	UPROPERTY(BlueprintAssignable)
	FOnXPChangedSignature OnXPPercentChangedDelegate; // Send XP% and Level to UI
protected:
	UFUNCTION()
	void BroadcastXPToUI(int32 XP = 0) const;
};
