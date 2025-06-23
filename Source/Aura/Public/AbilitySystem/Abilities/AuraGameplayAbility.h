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
	UPROPERTY(EditDefaultsOnly, Category="Inputs", meta=(GameplayTagFilter="Inputs"))
	FGameplayTag StartupInputTag;
protected:
	/*virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;*/
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr) override;
	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<ICombatInterface> AvatarCombatInterface;
	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<IEnemyInterface> AvatarEnemyInterface;
	UPROPERTY(BlueprintReadOnly)
	AActor* AvatarActor;
};
