// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "UI/Widget/AuraButtonWidget.h"
#include "SpellGlobeButtonWidget.generated.h"

class USpellMenuWidgetController;
class UImage;
struct FAuraAbilityData;
/**
 * 
 */
UCLASS()
class AURA_API USpellGlobeButtonWidget : public UAuraButtonWidget
{
	GENERATED_BODY()
public:
	virtual void SetWidgetController(UAuraWidgetController* InWidgetController) override;
	
	UFUNCTION(BlueprintCallable)
	void ReceiveAbilityData(const FAuraAbilityData& AbilityData,
		UTexture2D* LockedTexture, UMaterialInterface* LockedMaterial);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Properties")
	FMargin InPadding = FMargin(30.f); // Background, Glass, SpellIcon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Properties")
	FSlateBrush BackgroundBrush = FSlateBrush(); // Background, Glass, SpellIcon

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Properties", meta=(GameplayTagFilter="Ability"))
	FGameplayTag AbilityTag = FGameplayTag(); // Background, Glass, SpellIcon
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag StatusTag = AuraGameplayTags::Ability_Status_Locked;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USpellMenuWidgetController> SpellMenuWC;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_Background;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_SpellIcon;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_Glass;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_Selection;
protected:
	virtual void NativePreConstruct() override;
};
