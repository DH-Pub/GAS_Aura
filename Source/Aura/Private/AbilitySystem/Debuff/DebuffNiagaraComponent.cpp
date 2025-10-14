// Copyright Hung


#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"

#include "AbilitySystemComponent.h"

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;
}

void UDebuffNiagaraComponent::SetASC(UAbilitySystemComponent* InASC)
{
	DebuffTagDelegate = InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
}
void UDebuffNiagaraComponent::DisableNiagara(UAbilitySystemComponent* InASC)
{
	InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	Deactivate();
}

void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, const int32 NewCount)
{
	if (NewCount > 0) Activate();
	else Deactivate();
}
