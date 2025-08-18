// Copyright Hung


#include "UI/Widget/SpellGlobeButtonWidget.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Components/Image.h"
#include "UI/WidgetController/OverlayWidgetController.h"
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
		if (AbilityData.StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Locked))
		{
			Image_SpellIcon->SetBrushFromTexture(LockedTexture);
			Image_Background->SetBrushFromMaterial(LockedMaterial);
		}
		else if (AbilityData.StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Unlocked)
			|| AbilityData.StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Equipped))
		{
			Image_SpellIcon->SetBrushFromTexture(AbilityData.Icon);
			Image_Background->SetBrushFromMaterial(AbilityData.BackgroundMaterial);
		}
		else if (AbilityData.StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Eligible))
		{
			Image_SpellIcon->SetBrushFromTexture(AbilityData.Icon);
			Image_Background->SetBrushFromMaterial(LockedMaterial);
		}
	}
}
