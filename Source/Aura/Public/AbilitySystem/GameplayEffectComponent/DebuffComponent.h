// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "DebuffComponent.generated.h"

/**
 * modify from UAdditionalEffectsGameplayEffectComponent
 */
UCLASS(CollapseCategories, DisplayName="Aura Apply Additional Debuff")
class AURA_API UDebuffComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()
public:
	virtual bool OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer,
		FActiveGameplayEffect& ActiveGE) const override;
protected:
	void OnActiveGameplayEffectRemoved(const struct FGameplayEffectRemovalInfo& RemovalInfo,
		FActiveGameplayEffectsContainer* ActiveGEContainer) const;
public:
	/*// Other gameplay effects that will be applied to the target of this effect if the owning effect applies
	UPROPERTY(EditDefaultsOnly, Category = OnApplication)
	TSubclassOf<UGameplayEffect> OnApplicationDebuff;
	// Effects to apply when this effect completes, regardless of how it ends
	UPROPERTY(EditDefaultsOnly, Category = OnComplete)
	TSubclassOf<UGameplayEffect> OnCompleteDamageDebuff;*/
};
