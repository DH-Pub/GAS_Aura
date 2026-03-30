// Copyright Hung


#include "UI/Widget/Spells/SpellGlobeButtonWidget.h"

#include "AuraTag.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
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
	SpellMenuWC->GetASC()->AbilityDataDelegate.AddDynamic(this, &USpellGlobeButtonWidget::ReceiveAbilityData);
	Super::SetWidgetController(InWidgetController);
}

void USpellGlobeButtonWidget::ReceiveAbilityData(const FGameplayAbilitySpec& AbilitySpec,
	const FAuraAbilityData& Data)
{
	if (Data.AbilityClass != AbilityClass) return;

	const FGameplayTagContainer& Tags = AbilitySpec.GetDynamicSpecSourceTags();
	if (Tags.HasTagExact(AuraTag::Ability_Status_Locked))
	{
		StatusTag = AuraTag::Ability_Status_Locked;
		bDragEnable = false;
		Image_SpellIcon->SetBrushFromTexture(LockedTexture);
		Image_Background->SetBrushFromMaterial(LockedMaterial);
	}
	else if (Tags.HasTagExact(AuraTag::Ability_Status_Eligible))
	{
		StatusTag = AuraTag::Ability_Status_Eligible;
		bDragEnable = false;
		Image_SpellIcon->SetBrushFromTexture(Data.Icon);
		Image_Background->SetBrushFromMaterial(LockedMaterial);
	}
	else
	{	// not under any status
		StatusTag = FGameplayTag::EmptyTag;
		bDragEnable = true;
		Image_SpellIcon->SetBrushFromTexture(Data.Icon);
		Image_Background->SetBrushFromMaterial(Data.BackgroundMaterial);
	}

	// Order: GiveAbility => Updating SpellPoints, but AbilityDataDelegate broadcast slower than OnSpellPointChangedDelegate
	// Equipped Spell will not activate this on SpellPointsChanged
	if (SpellMenuWC->FocusSpellGlobe == this) SpellMenuWC->UpdateButtonsAndDescriptions();
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
	if (SpellMenuWC) SpellMenuWC->GetASC()->AbilityDataDelegate.RemoveAll(this);
	Super::NativeDestruct();
}


void USpellGlobeButtonWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	Border->SetBrush(BorderHovered);
	SetFocus(); // NativeOnFocusReceived()
}
void USpellGlobeButtonWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	Border->SetBrush(BorderNormal);
	if (SpellMenuWC->SelectedSpellGlobe) SpellMenuWC->SelectedSpellGlobe->SetFocus(); // Change Focus to Selected
}

FReply USpellGlobeButtonWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (!SpellMenuWC) return Super::NativeOnFocusReceived(InGeometry,InFocusEvent);
	UGameplayStatics::PlaySound2D(this, ClickSound);
	if (SpellMenuWC->SelectedSpellGlobe != this)
	{
		Image_Selection->SetVisibility(ESlateVisibility::Visible);
		PlayAnimation(SelectAnimation);
	}
	SpellMenuWC->FocusSpellGlobe = this;
	SpellMenuWC->UpdateButtonsAndDescriptions(false);
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
