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
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite)
	int32 AbilityID = INDEX_NONE;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag AbilityTag;
private:
	UPROPERTY(EditAnywhere,Category="Default|GlobeProperties", meta=(EditCondition="PassiveID==EAuraAbilityPassiveID::None"))
	EAuraAbilityInputID InputID = EAuraAbilityInputID::None;
	UPROPERTY(EditAnywhere,Category="Default|GlobeProperties", meta=(EditCondition="InputID==EAuraAbilityInputID::None"))
	EAuraAbilityPassiveID PassiveID = EAuraAbilityPassiveID::None;
};
