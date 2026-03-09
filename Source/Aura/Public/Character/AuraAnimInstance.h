// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Animation/AnimInstance.h"
#include "AuraAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UAuraAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UAuraAnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	/**
	 * - Logic in the Event Graph is processed on the Game Thread.
	 * - Every tick, the Event Graph for each AnimBP must be run one after the other in sequence, which can be a performance bottleneck.
	 * - For this project, we've instead used the new BlueprintThreadsafeUpdateAnimation function (found in the My Blueprint tab).
	 * - Logic in BlueprintThreadsafeUpdateAnimation can be run in parallel for multiple AnimBP's simultaneously, removing the overhead on the Game Thread.
	 * - In Blueprint, search "Property Access"
	 */
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	// Called in InitializeAnimation and InitAbilityActorInfo()
	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AAuraCharacterBase> AuraCharacter;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UAuraMovementComponent> MovementComponent;

	// Gameplay tags that can be mapped to blueprint variables, which will automatically update as the tags are added or removed.
	// These should be used instead of manually querying for the gameplay tags.
	UPROPERTY(EditDefaultsOnly, Category = "Aura")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;


#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
