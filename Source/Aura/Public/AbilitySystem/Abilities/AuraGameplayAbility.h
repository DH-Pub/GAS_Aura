// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

class ICombatInterface;

UENUM(BlueprintType)
enum EOutcomeCombatInterface
{
	Success,
	Failure,
};
/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category="Inputs", meta=(Categories="Inputs"))
	FGameplayTag StartupInputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	FScalableFloat Damage;
	
	UFUNCTION(BlueprintCallable, meta=(ExpandEnumAsExecs="Outcome"))
	void GetAvatarCombatInterface(TScriptInterface<ICombatInterface>& CombatInterface, TEnumAsByte<EOutcomeCombatInterface>& Outcome);
private:
	UPROPERTY()
	TScriptInterface<ICombatInterface> AvatarCombatInterface;
};
