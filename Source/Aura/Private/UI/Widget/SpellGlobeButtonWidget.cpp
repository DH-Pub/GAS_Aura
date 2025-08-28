// Copyright Hung


#include "UI/Widget/SpellGlobeButtonWidget.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

void USpellGlobeButtonWidget::SetWidgetController(UAuraWidgetController* InWidgetController)
{
	SpellMenuWC = Cast<USpellMenuWidgetController>(InWidgetController);
	Super::SetWidgetController(InWidgetController);
}

void USpellGlobeButtonWidget::ReceiveAbilityData(const FAuraAbilityData& AbilityData, UTexture2D* LockedTexture,
                                                 UMaterialInterface* LockedMaterial)
{
	if (AbilityData.AbilityTag.MatchesTagExact(AbilityTag))
	{
		StatusTag = AbilityData.StatusTag;
		if (StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Locked))
		{
			Image_SpellIcon->SetBrushFromTexture(LockedTexture);
			Image_Background->SetBrushFromMaterial(LockedMaterial);
		}
		else if (StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Unlocked)
			|| StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Equipped))
		{
			Image_SpellIcon->SetBrushFromTexture(AbilityData.Icon);
			Image_Background->SetBrushFromMaterial(AbilityData.BackgroundMaterial);
		}
		else if (StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Eligible))
		{
			Image_SpellIcon->SetBrushFromTexture(AbilityData.Icon);
			Image_Background->SetBrushFromMaterial(LockedMaterial);
		}

		/*if (SpellMenuWC->HoveredSpellGlobe)
		{
			SpellMenuWC->UpdateSpendEquipButtons(SpellMenuWC->SpellPoints);
		}*/
	}
}

void USpellGlobeButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	Cast<UOverlaySlot>(Image_Glass->Slot)->SetPadding(InPadding);
	Cast<UOverlaySlot>(Image_Background->Slot)->SetPadding(InPadding);
	Cast<UOverlaySlot>(Image_SpellIcon->Slot)->SetPadding(InPadding);

	Image_Background->SetBrush(BackgroundBrush);
	Image_Selection->SetVisibility(ESlateVisibility::Collapsed);
}
