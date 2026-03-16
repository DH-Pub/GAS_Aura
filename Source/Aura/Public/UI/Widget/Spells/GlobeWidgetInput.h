// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GlobeWidget.h"
#include "GlobeWidgetInput.generated.h"

namespace EAuraAbilityInputID {enum Type : uint8;}

/**
 * Has FGameplayAbilitySpec::InputID (AbilityID), USpellGlobeWidget and UMenuEquipSpellWidget inherit from this
 */
UCLASS()
class AURA_API UGlobeWidgetInput : public UGlobeWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura") // EditDefaultOnly: Can't edit in Parent Instance
	TEnumAsByte<EAuraAbilityInputID::Type> AbilityID;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<class UGameplayAbility> AbilityClass;

	UFUNCTION(BlueprintCallable)
	virtual void ClearGlobe();
};
