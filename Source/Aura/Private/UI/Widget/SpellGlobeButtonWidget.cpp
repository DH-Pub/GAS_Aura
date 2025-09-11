// Copyright Hung


#include "UI/Widget/SpellGlobeButtonWidget.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

void USpellGlobeButtonWidget::SetWidgetController(UAuraWidgetController* InWidgetController)
{
	SpellMenuWC = Cast<USpellMenuWidgetController>(InWidgetController);
	SpellMenuWC->AbilitySystemComponent->AbilityDataDelegate.AddDynamic(this, &USpellGlobeButtonWidget::ReceiveAbilityData);
	Super::SetWidgetController(InWidgetController);
}

void USpellGlobeButtonWidget::ReceiveAbilityData(const FAuraAbilityData& AbilityData, const FPlayerAbilityData& PlayerData)
{
	if (!AbilityData.AbilityTag.MatchesTagExact(AbilityTag)) return;
	StatusTag = PlayerData.StatusTag;
	if (StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Locked))
	{
		bDragEnable = false;
		Image_SpellIcon->SetBrushFromTexture(LockedTexture);
		Image_Background->SetBrushFromMaterial(LockedMaterial);
	}
	else if (StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Eligible))
	{
		bDragEnable = false;
		Image_SpellIcon->SetBrushFromTexture(AbilityData.Icon);
		Image_Background->SetBrushFromMaterial(LockedMaterial);
	}
	else if (StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Unlocked))
	{
		bDragEnable = true;
		Image_SpellIcon->SetBrushFromTexture(AbilityData.Icon);
		Image_Background->SetBrushFromMaterial(AbilityData.BackgroundMaterial);
	}

	// Order: GiveAbility before Updating SpellPoints
	// However AbilityDataDelegate broadcast slower than OnSpellPointChangedDelegate
	// Equipped Spell will not activate this on SpellPointsChanged
	if (SpellMenuWC->FocusSpellGlobe == this)
	{
		SpellMenuWC->UpdateButtonsAndDescriptions(SpellMenuWC->SpellPoints, AbilityTag, StatusTag);
	}
}

void USpellGlobeButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SizeBox_Root->SetWidthOverride(ButtonWidthHeight.X);
	SizeBox_Root->SetHeightOverride(ButtonWidthHeight.Y);

	Cast<UOverlaySlot>(Image_Glass->Slot)->SetPadding(InPadding);
	Cast<UOverlaySlot>(Image_Background->Slot)->SetPadding(InPadding);
	Cast<UOverlaySlot>(Image_SpellIcon->Slot)->SetPadding(InPadding);

	Image_Background->SetBrushFromMaterial(LockedMaterial);
	Border->SetBrush(BorderNormal);
	Image_Selection->SetVisibility(ESlateVisibility::Collapsed);
}
void USpellGlobeButtonWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (SpellMenuWC) SpellMenuWC->AbilitySystemComponent->AbilityDataDelegate.RemoveAll(this);
}


void USpellGlobeButtonWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	Border->SetBrush(BorderHovered);
	SetFocus();
}
void USpellGlobeButtonWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	Border->SetBrush(BorderNormal);
	if (SpellMenuWC->SelectedSpellGlobe) SpellMenuWC->SelectedSpellGlobe->SetFocus(); // Change Focus to old
}

FReply USpellGlobeButtonWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	UGameplayStatics::PlaySound2D(this, ClickSound);
	if (SpellMenuWC->SelectedSpellGlobe != this)
	{
		Image_Selection->SetVisibility(ESlateVisibility::Visible);
		PlayAnimation(SelectAnimation);
	}
	SpellMenuWC->FocusSpellGlobe = this;
	SpellMenuWC->UpdateButtonsAndDescriptions(SpellMenuWC->SpellPoints, AbilityTag, StatusTag, false);
	FEventReply Reply(true);
	return UWidgetBlueprintLibrary::SetUserFocus(Reply, this).NativeReply;
}
void USpellGlobeButtonWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);
	if (SpellMenuWC->SelectedSpellGlobe != this) Image_Selection->SetVisibility(ESlateVisibility::Collapsed);
}


FReply USpellGlobeButtonWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Border->SetBrush(BorderClicked);
	if (SpellMenuWC->SelectedSpellGlobe) SpellMenuWC->SelectedSpellGlobe->Image_Selection->SetVisibility(ESlateVisibility::Collapsed);
	SpellMenuWC->SelectedSpellGlobe = this;
	UGameplayStatics::PlaySound2D(this, ClickSound);
	if (bDragEnable)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this,
			EKeys::LeftMouseButton).NativeReply;
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
FReply USpellGlobeButtonWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Border->SetBrush(BorderHovered);
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}
