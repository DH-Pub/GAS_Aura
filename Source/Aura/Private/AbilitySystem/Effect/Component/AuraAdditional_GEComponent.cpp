// Copyright Hung


#include "AbilitySystem/Effect/Component/AuraAdditional_GEComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Misc/DataValidation.h"

bool UAuraAdditional_GEComponent::OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer,
	FActiveGameplayEffect& ActiveGE) const
{
	if (ActiveGEContainer.IsNetAuthority()) // We don't allow prediction of expiration (on removed) effects
	{	// When this ActiveGE gets removed, so will our events so no need to unbind this.
		ActiveGE.EventSet.OnEffectRemoved.AddUObject(this,
			&UAuraAdditional_GEComponent::OnActiveGameplayEffectRemoved, &ActiveGEContainer);
	}
	return true;
}

void UAuraAdditional_GEComponent::OnGameplayEffectExecuted(FActiveGameplayEffectsContainer& ActiveGEContainer,
	FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{

}

void UAuraAdditional_GEComponent::OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer,
	FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	if (!ensureMsgf(ActiveGEContainer.Owner, TEXT("ASC is somehow null"))) return;

	UAbilitySystemComponent& AppliedToASC = *ActiveGEContainer.Owner;
	for (const TSubclassOf Effect : GlobalCueEffects)
	{
		if (const UGameplayEffect* CDO = Effect.GetDefaultObject())
		{
			FGameplayEffectSpecHandle SpecHandle = FGameplayEffectSpecHandle(new FGameplayEffectSpec());
			SpecHandle.Data->InitializeFromLinkedSpec(CDO, GESpec); // This doesn't copy Period, so no need to check
			if (ensure(SpecHandle.IsValid()))
			{
				AppliedToASC.ApplyGameplayEffectSpecToSelf(*SpecHandle.Data, PredictionKey);
			}
		}
	}
}

void UAuraAdditional_GEComponent::OnActiveGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo,
	FActiveGameplayEffectsContainer* ActiveGEContainer) const
{
	UAbilitySystemComponent* ASC = ActiveGEContainer->Owner;
	for (const TSubclassOf Effect : GlobalCueEffects)
	{
		ASC->RemoveActiveGameplayEffectBySourceEffect(Effect, nullptr, RemovalInfo.StackCount);
	}
}


#define LOCTEXT_NAMESPACE "AdditionalEffectsGameplayEffectComponent"

#if WITH_EDITOR
EDataValidationResult UAuraAdditional_GEComponent::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (GetOwner()->DurationPolicy == EGameplayEffectDurationType::Instant)
	{
		Context.AddError(FText::FormatOrdered(LOCTEXT("InstantDoesNotWorkForAddOrRemoveCue",
			"Can't use Instant GE for Add/Remove Cue for {0}."), FText::FromString(GetClass()->GetName())));
		Result = EDataValidationResult::Invalid;
	}
	if (GlobalCueEffects.IsEmpty())
	{
		Context.AddError(FText::FormatOrdered(LOCTEXT("EffectsCantBeEmpty",
			"Effects can't be empty {0}."), FText::FromString(GetClass()->GetName())));
		Result = EDataValidationResult::Invalid;
	}
	for (const TSubclassOf EffectClass : GlobalCueEffects)
	{
		if (!EffectClass)
		{
			Context.AddError(FText::FormatOrdered(LOCTEXT("EffectClassHasToBeValid",
				"EffectClass has to be valid {0}."), FText::FromString(GetClass()->GetName())));
			Result = EDataValidationResult::Invalid;
		}
		if (const UGameplayEffect* CDO = EffectClass.GetDefaultObject())
		{
			if (CDO->DurationPolicy != EGameplayEffectDurationType::Infinite || CDO->Period != 0.f)
			{
				Context.AddError(FText::FormatOrdered(LOCTEXT("EffectInvalid",
					"Effect must be Infinite, non-periodic, and stackable {0}."), FText::FromString(GetClass()->GetName())));
				Result = EDataValidationResult::Invalid;
			}
			if (CDO == GetOwner())
			{
				Context.AddError(FText::FormatOrdered(LOCTEXT("EffectInvalid",
					"Effect can't re-apply self {0}."), FText::FromString(GetClass()->GetName())));
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE
