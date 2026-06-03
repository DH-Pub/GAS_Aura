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
	SpellMenuWC->OnReceiveAbilityDataFromASC.AddDynamic(this, &UMenuEquipSpellWidget::UpdateAbilityUI);
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
	if (SpellMenuWC) SpellMenuWC->OnReceiveAbilityDataFromASC.RemoveAll(this);
	Super::NativeDestruct();
}

void UMenuEquipSpellWidget::UpdateAbilityUI()
{
	if (const FAuraAbilityData* Data = UAbilityDataAsset::GetAbilityDataFromID(SpellMenuWC->GetASC(), AbilityID))
	{
		AbilityClass = Data->AbilityClass;

		Image_SpellIcon->SetBrushTintColor(FLinearColor::White); // ClearGlobe turns Tint Alpha to 0
		Image_SpellIcon->SetBrushResourceObject(Data->Icon);

		Image_Background->SetBrushTintColor(FLinearColor::White);
		Image_Background->SetBrushResourceObject(Data->BackgroundMaterial);
	}
	else ClearGlobe();
}
