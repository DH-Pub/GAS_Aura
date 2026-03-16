// Copyright Hung


#include "UI/Widget/Spells/MenuEquipSpellWidget.h"

#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

void UMenuEquipSpellWidget::SetWidgetController(UAuraWidgetController* InWidgetController)
{
	SpellMenuWC = Cast<USpellMenuWidgetController>(InWidgetController);
	SpellMenuWC->GetASC()->AbilityDataDelegate.AddDynamic(this, &UMenuEquipSpellWidget::ReceiveAbilityData);
	Super::SetWidgetController(InWidgetController);
}

void UMenuEquipSpellWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	Cast<UOverlaySlot>(Image_SpellIcon->Slot)->SetPadding(InPadding);
	ClearGlobe();
}

void UMenuEquipSpellWidget::NativeDestruct()
{
	if (SpellMenuWC) SpellMenuWC->GetASC()->AbilityDataDelegate.RemoveAll(this);
	Super::NativeDestruct();
}

void UMenuEquipSpellWidget::ClearGlobe()
{
	Super::ClearGlobe();

	CurrentIcon = CurrentBackground = DefaultBackground;
	Image_Background->SetBrush(CurrentBackground);
	Image_SpellIcon->SetBrush(CurrentIcon);
}

void UMenuEquipSpellWidget::ReceiveAbilityData(const FGameplayAbilitySpec& AbilitySpec, const FAuraAbilityData& Data)
{
	if (AbilitySpec.InputID == AbilityID)
	{
		AbilityClass = Data.AbilityClass;

		FSlateBrush Brush;
		Brush.SetResourceObject(Data.Icon);
		CurrentIcon = Brush; // UWidgetBlueprintLibrary::MakeBrushFromTexture
		Image_SpellIcon->SetBrush(Brush);

		Brush.SetResourceObject(Data.BackgroundMaterial);
		CurrentBackground = Brush; // UWidgetBlueprintLibrary::MakeBrushFromMaterial
		Image_Background->SetBrush(Brush);
	}
	else if (Data.AbilityClass == AbilityClass) ClearGlobe();
}
