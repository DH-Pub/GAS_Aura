// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "DebuffNiagaraComponent.generated.h"

class UAbilitySystemComponent;
/**
 *
 */
UCLASS()
class AURA_API UDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
public:
	UDebuffNiagaraComponent();
	void SetASC(UAbilitySystemComponent* InASC);
	void DisableNiagara(UAbilitySystemComponent* InASC);

	UPROPERTY(VisibleAnywhere, Category="Default", meta=(GameplayTagFilter="Debuff.Type."))
	FGameplayTag DebuffTag;
protected:
	void DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
private:
	FDelegateHandle DebuffTagDelegate;
};
