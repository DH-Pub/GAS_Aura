// Copyright Hung


#include "AbilitySystem/Abilities/CostCooldownAbility.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/ModMagCalc/MMC_Utility.h"
#include "DSP/SpectrumAnalyzer.h"
#include "Player/AuraPlayerController.h"

UCostCooldownAbility::UCostCooldownAbility()
{
	FGameplayTagContainer DefaultAssetTags;
	SetAssetTags(AddGenericAssetTags(DefaultAssetTags));
	SetGenericCancelBlockAbility();
	// ActivationOwnedTags.AddTag(AuraGameplayTags::Ability_Fire_FireBolt);
	ActivationBlockedTags.AddTag(AuraGameplayTags::Ability_HitReact);
	// ActivationRequiredTags.AddTag();
	bRetriggerInstancedAbility = true;
}

const FGameplayTagContainer* UCostCooldownAbility::GetCooldownTags() const
{
	if (!CooldownTags.IsValid()) return Super::GetCooldownTags();

	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // MutableTags writes to the TempCooldownTags on the CDO so clear it in case the cooldown tags change (to a different slot)
	if (const FGameplayTagContainer* ParentTags = Super::GetCooldownTags())
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	return MutableTags;
}

void UCostCooldownAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (!CooldownTags.IsValid()) {Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo); return;}
	
	if (const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect())
	{
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		SpecHandle.Data.Get()->SetByCallerTagMagnitudes.FindOrAdd(CooldownTags.GetByIndex(0))
			= CooldownDuration.GetValueAtLevel(GetAbilityLevel());
		// Use MMC
		FActiveGameplayEffectHandle ActiveEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

void UCostCooldownAbility::GetCost(FAbilityDetails& Details) const
{
	if (const UGameplayEffect* CostGE = GetCostGameplayEffect())
	{
		for (const FGameplayModifierInfo& Mod : CostGE->Modifiers)
		{
			if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), Details.Level);
				Mod.ModifierMagnitude.AttemptCalculateMagnitude(*SpecHandle.Data, Details.CostMana);
				Details.CostMana = static_cast<int32>(Details.CostMana * -10.f) / 10.f;
				// Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, Mana);
			}
			else if (Mod.Attribute == UAuraAttributeSet::GetHealthAttribute())
			{
				FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), Details.Level);
				Mod.ModifierMagnitude.AttemptCalculateMagnitude(*SpecHandle.Data, Details.CostHealth);
				Details.CostHealth = static_cast<int32>(Details.CostHealth * -10.f) / 10.f;
			}
		}
	}
}
void UCostCooldownAbility::GetCooldownAndReduction(FAbilityDetails& Details) const
{
	if (const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect())
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), Details.Level);
		const FGameplayEffectModifierMagnitude& ModMag = CooldownGE->DurationMagnitude;
		// const UGameplayModMagnitudeCalculation* CalcCDO = ModMag.GetCustomMagnitudeCalculationClass()->GetDefaultObject<UGameplayModMagnitudeCalculation>();
		if (const UMMC_CooldownDuration* CalcCDO = ModMag.GetCustomMagnitudeCalculationClass()->GetDefaultObject<UMMC_CooldownDuration>())
		{
			const TPair<float, float> CD = CalcCDO->GetBaseCooldownAndReductionPercent(*SpecHandle.Data);
			Details.BaseCooldown = static_cast<int32>(CD.Key * 10.f) / 10.f;
			Details.CooldownReduction = CD.Value;

			Details.CalculatedCooldown = Details.BaseCooldown * (1 - Details.CooldownReduction);
			Details.CalculatedCooldown = static_cast<int32>(Details.CalculatedCooldown * 10.f) / 10.f;
		}
	}
}
void UCostCooldownAbility::GetAbilityDetailsCostCooldown(FAbilityDetails& Details) const
{
	GetCost(Details);
	GetCooldownAndReduction(Details);
}


/*
 * Input ===============================================================================================================
 */
#pragma region Input ===============================================
void UCostCooldownAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	//TODO: Find sth to do with this
	if (AbilityTriggerEvent == ETriggerEvent::Canceled) TapReleased();
	else if (AbilityTriggerEvent == ETriggerEvent::Completed) HoldReleased();
}
FInputActionValue UCostCooldownAbility::GetBoundAuraActionValue() const
{
	if (AuraPlayerController) return AuraPlayerController->AuraInputComponent->BindActionValue(InputAction).GetValue();
	return FInputActionValue();
}

void UCostCooldownAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	switch (AbilityTriggerEvent)
	{
	case ETriggerEvent::Started:
		StartPressedOngoing();
		break;
	case ETriggerEvent::Triggered:
		if (!bStartHold)
		{
			bStartHold = true;
			StartHoldTriggered();
		}
		break;
	default: break;	
	}
	
	/*GEngine->AddOnScreenDebugMessage(100, 1.f, FColor::Cyan,
		FString::Printf(TEXT("%s"), *UEnum::GetValueAsString(AbilityTriggerEvent)));*/
}

void UCostCooldownAbility::StartPressedOngoing_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(RepeatDelayTimer);
	switch (++ClickNums)
	{
	case 2: DoubleClick(); break;
	case 3: TripleClick(); break;
	default: break;
	}
	if (ClickNums >= 3) {ClickNums = 0;}
}
void UCostCooldownAbility::StartHoldTriggered_Implementation(){}
void UCostCooldownAbility::TapReleased_Implementation(){}
void UCostCooldownAbility::HoldReleased_Implementation(){}

void UCostCooldownAbility::DoubleClick_Implementation(){}
void UCostCooldownAbility::TripleClick_Implementation(){}

void UCostCooldownAbility::SetAbilityTriggerEvent(const ETriggerEvent TriggerEvent)
{
	if (AbilityTriggerEvent == TriggerEvent) return;
	
	AbilityTriggerEvent = TriggerEvent;
	switch (TriggerEvent)
	{
	case ETriggerEvent::Triggered:
		ClickNums = 0;
		break;
	case ETriggerEvent::Canceled:
		// FTimerDelegate TimerDelegate; TimerDelegate.BindLambda([this](){AbilityTriggerEvent = ETriggerEvent::Ongoing;});
		// FTimerDelegate TimerDelegate; TimerDelegate.BindUFunction(this, "HoldThresholdReached", ActorInfo, TriggerEventData);
		GetWorld()->GetTimerManager().SetTimer(RepeatDelayTimer, FTimerDelegate::CreateLambda([this]
		{
			ClickNums = 0;
		}),  RepeatDelayTime, false);
		break;
	case ETriggerEvent::Completed:
		bStartHold = false;
		break;
	default: break;
	}
}
#pragma endregion
