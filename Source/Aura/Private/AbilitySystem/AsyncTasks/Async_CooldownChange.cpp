// Copyright Hung


#include "AbilitySystem/AsyncTasks/Async_CooldownChange.h"

#include "AbilitySystemComponent.h"

UAsync_CooldownChange* UAsync_CooldownChange::WaitForCooldownChange(
	UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag)
{
	UAsync_CooldownChange* WaitCooldownChange = NewObject<UAsync_CooldownChange>();
	WaitCooldownChange->ASC = AbilitySystemComponent;
	WaitCooldownChange->CooldownTag = InCooldownTag;
	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask();
		return nullptr;
	}

	// Cooldown Tag Removed. Recommended because when the Server's corrected Cooldown GE comes in,
	// it will remove our locally predicted one causing the OnAnyGameplayEffectRemovedDelegate() to fire even though we're still on cooldown.
	// The Cooldown Tag will not change during the removal of the predicted Cooldown GE and the application of the Server's corrected Cooldown GE.
	AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved)
	.AddUObject(WaitCooldownChange /* In a static function, 'this' means nothing */, &UAsync_CooldownChange::CooldownTagChanged);
	
	// CD GE Applied. Recommended because you also have access to the GameplayEffectSpec that applied it.
	// From this you can determine if the Cooldown GE is the locally predicted one or the Server's correcting one.
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &UAsync_CooldownChange::OnActiveEffectAdded);
	
	return WaitCooldownChange;
}

void UAsync_CooldownChange::EndTask()
{
	if (!IsValid(ASC)) return;
	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	ASC->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
	
	SetReadyToDestroy();
	MarkAsGarbage();
}

void UAsync_CooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, const int32 NewCount)
{
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}
	/*else
	{
		const FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTag.GetSingleTagContainer());
		const TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
		CooldownStart.Broadcast(FMath::Max(TimesRemaining)); // if empty, returns 0;
	}*/
}

void UAsync_CooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);
	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);
	
	// SpecApplied.GetContext().GetAbilityInstance_NotReplicated();
	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
	{
		const FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		const TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
		// const TArray<TPair<float, float>> TimesRemaining_Duration = ASC->GetActiveEffectsTimeRemainingAndDuration(GameplayEffectQuery);
		const float Remaining = FMath::Max(TimesRemaining);
		if (Remaining > 0.f)
		{
			CooldownStart.Broadcast(Remaining); // if empty, returns 0;
		}
	}
}
