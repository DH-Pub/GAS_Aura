// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

class IEnemyInterface;
class ICombatInterface;

UENUM(BlueprintType)
enum EOutcome
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
	UAuraGameplayAbility();
	UPROPERTY(EditDefaultsOnly, Category="Inputs", meta=(Categories="Inputs"))
	FGameplayTag StartupInputTag;
	
	UFUNCTION(BlueprintCallable, meta=(ExpandEnumAsExecs="Outcome"))
	void GetAvatarCombatInterface(TScriptInterface<ICombatInterface>& CombatInterface, TEnumAsByte<EOutcome>& Outcome);
	UFUNCTION(BlueprintCallable, meta=(ExpandEnumAsExecs="Outcome"))
	void GetAvatarInterfaces(TEnumAsByte<EOutcome>& Outcome, TScriptInterface<ICombatInterface>& CombatInterface,
		TScriptInterface<IEnemyInterface>& EnemyInterface);
private:
	UPROPERTY()
	TScriptInterface<ICombatInterface> AvatarCombatInterface;
	UPROPERTY()
	TScriptInterface<IEnemyInterface> AvatarEnemyInterface;
};
