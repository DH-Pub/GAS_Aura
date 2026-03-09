// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GlobeWidget.h"
#include "Aura/Aura.h"
#include "GlobeWidgetInput.generated.h"

/**
 * Has FGameplayAbilitySpec::InputID (AbilityID), USpellGlobeWidget and UMenuEquipSpellWidget inherit from this
 */
UCLASS()
class AURA_API UGlobeWidgetInput : public UGlobeWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura") // EditDefaultOnly: Can't edit in Parent Instance
	TEnumAsByte<EAuraAbilityInputID::Type> AbilityID = EAuraAbilityInputID::None;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag AbilityTag;
};
