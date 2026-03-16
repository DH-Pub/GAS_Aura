// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraTag.h"
#include "GameplayTagContainer.h"
#include "GlobeWidget.h"
#include "SpellGlobeButtonWidget.generated.h"

class UImage;
/**
 *
 */
UCLASS()
class AURA_API USpellGlobeButtonWidget : public UGlobeWidget
{
	GENERATED_BODY()
public:
	virtual void SetWidgetController(UAuraWidgetController* InWidgetController) override;

	UFUNCTION(BlueprintCallable)
	void ReceiveAbilityData(const struct FGameplayAbilitySpec& AbilitySpec, const struct FAuraAbilityData& AbilityData);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aura|Properties")
	FVector2D ButtonWidthHeight = FVector2D(40., 40.);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura", meta=(GameplayTagFilter="Ability"))
	TSubclassOf<class UAuraGameplayAbility> AbilityClass; // To compare and get
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag StatusTag = AuraTag::Ability_Status_Locked;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class USpellMenuWidgetController> SpellMenuWC;

#pragma region Widget
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_Selection;
#pragma endregion
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_SpellIcon;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<class UBorder> Border;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Border")
	FSlateBrush BorderDisabled = FSlateBrush();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Border")
	FSlateBrush BorderNormal = FSlateBrush();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Border")
	FSlateBrush BorderHovered = FSlateBrush();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Border")
	FSlateBrush BorderClicked = FSlateBrush();

	UPROPERTY(BlueprintReadWrite, Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SelectAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura")
	TObjectPtr<USoundBase> ClickSound;
private:
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UTexture2D> LockedTexture;
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UMaterialInterface> LockedMaterial;
	bool bDragEnable = false;
};
