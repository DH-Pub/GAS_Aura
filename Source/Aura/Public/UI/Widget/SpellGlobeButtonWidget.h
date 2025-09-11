// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "AuraUserWidget.h"
#include "GameplayTagContainer.h"
#include "SpellGlobeButtonWidget.generated.h"

struct FPlayerAbilityData;
class USpellMenuWidgetController;
class UImage;
struct FAuraAbilityData;
/**
 * 
 */
UCLASS()
class AURA_API USpellGlobeButtonWidget : public UAuraUserWidget
{
	GENERATED_BODY()
public:
	virtual void SetWidgetController(UAuraWidgetController* InWidgetController) override;

	UFUNCTION(BlueprintCallable)
	void ReceiveAbilityData(const FAuraAbilityData& AbilityData, const FPlayerAbilityData& PlayerData);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Properties")
	FVector2D ButtonWidthHeight = FVector2D(40., 40.);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Properties")
	FMargin InPadding = FMargin(30.f); // Background, Glass, SpellIcon

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta=(GameplayTagFilter="Ability"))
	FGameplayTag AbilityTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag StatusTag = AuraGameplayTags::Ability_Status_Locked;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USpellMenuWidgetController> SpellMenuWC;

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

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional)) 
	TObjectPtr<class USizeBox> SizeBox_Root;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_Background;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_SpellIcon;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_Glass;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<class UBorder> Border;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|Border")
	FSlateBrush BorderDisabled = FSlateBrush();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|Border")
	FSlateBrush BorderNormal = FSlateBrush();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|Border")
	FSlateBrush BorderHovered = FSlateBrush();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|Border")
	FSlateBrush BorderClicked = FSlateBrush();

	UPROPERTY(BlueprintReadWrite, Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SelectAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	TObjectPtr<USoundBase> ClickSound;
private:
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<UTexture2D> LockedTexture;
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<UMaterialInterface> LockedMaterial;
	bool bDragEnable = false;
};
