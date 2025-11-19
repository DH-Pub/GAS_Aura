// Copyright Hung


#include "UI/Widget/Spells/GlobeWidgetInput.h"

#include "AbilitySystem/Ability/AuraGameplayAbility.h"

void UGlobeWidgetInput::NativeConstruct()
{
	AbilityID = UAuraGameplayAbility::ConvertInputAndPassiveEnumToAbilityID(InputID, PassiveID);

	Super::NativeConstruct(); // BP_Construct is called in here so call this last
}
