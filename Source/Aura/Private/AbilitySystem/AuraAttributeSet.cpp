// Copyright Hung


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h" // DOREPLIFETIME_CONDITION_NOTIFY
#include "GameplayEffectExtension.h" // FGameplayEffectModCallbackData.EvaluatedData
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Effect/VitalsResetEffect.h"
#include "Character/AuraCharacterBase.h"
#include "Game/AuraGameMode.h"
#include "Player/AuraPlayerState.h"

FEffectProperties::FEffectProperties(const FGameplayEffectModCallbackData& Data)
{
	SourceASC = Data.EffectSpec.GetEffectContext().GetOriginalInstigatorAbilitySystemComponent();
	if (IsValid(SourceASC) && SourceASC->AbilityActorInfo.IsValid())
	{
		SourceCharacter = Cast<AAuraCharacterBase>(SourceASC->AbilityActorInfo->AvatarActor.Get());
		SourceController = SourceASC->AbilityActorInfo->PlayerController.Get();
		if (SourceController && !SourceCharacter) SourceCharacter = Cast<AAuraCharacterBase>(SourceController->GetPawn());
		else if (!SourceController && SourceCharacter) SourceController = SourceCharacter->GetController();
	}

	// Target should be the owner of this AttributeSet
	const TSharedPtr<FGameplayAbilityActorInfo> TargetAbilityActorInfo = Data.Target.AbilityActorInfo;
	if (TargetAbilityActorInfo.IsValid() && TargetAbilityActorInfo->AvatarActor.IsValid())
	{
		TargetController = TargetAbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<AAuraCharacterBase>(TargetAbilityActorInfo->AvatarActor.Get());
	}
}


UAuraAttributeSet::UAuraAttributeSet()
{
}

// To Replicate
#pragma region ReplicateAttribute
void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Primary
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	// Secondary
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	// Resistance
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);

	// Vital
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);

	// Meta
	// DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, IncomingDamage, COND_None, REPNOTIFY_Always)
	// DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, IncomingXP, COND_None, REPNOTIFY_Always)
}

#pragma endregion


void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{	// can only affect CurrentValue (temporary)
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	else if (Attribute == GetManaAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
}
// Each Attribute has BaseValue and CurrentValue
void UAuraAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{	// affect the BaseValue (permanent), includes CurrentValue
	Super::PreAttributeBaseChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	else if (Attribute == GetManaAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, const float OldValue, const float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	// float CurrentPercent = GetHealth()/OldValue; // If there is a change in MaxValue, Keep percentage of Vitals
	if (Attribute == GetMaxHealthAttribute()) SetHealth(GetHealth() * GetMaxHealth()/OldValue);
	else if (Attribute == GetMaxManaAttribute()) SetMana(GetMana() * GetMaxMana()/OldValue);
}


// Called just before BaseValue is changed, can also be used to clamp
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& DataAttribute = Data.EvaluatedData.Attribute;
	const FEffectProperties Props(Data);
	if (DataAttribute == GetIncomingDamageAttribute()) HandleIncomingDamage(Data, Props);
	else if (DataAttribute == GetIncomingXPAttribute()) HandleIncomingXP(Data, Props);
}

void UAuraAttributeSet::HandleIncomingDamage(const FGameplayEffectModCallbackData& Data, const FEffectProperties& Props)
{
	const float LocalIncomingDamage = GetIncomingDamage(); // SetIncomingDamage(0.f); // Old Damage overriden in ExecCalc_Damage
	if (LocalIncomingDamage < UE_KINDA_SMALL_NUMBER) return;
	const float OldHealth = GetHealth();
	SetHealth(FMath::Clamp(OldHealth - LocalIncomingDamage, 0.f, GetMaxHealth()));

	if (GetHealth() < UE_KINDA_SMALL_NUMBER)
	{
		if (const AAuraGameMode* GameMode = AAuraGameMode::Get(this))
		{
			const FGameplayEffectSpecHandle SpecHandle = Props.SourceASC->MakeOutgoingSpec(
				GameMode->DeathEffect, 1.f, Data.EffectSpec.GetContext()); // Props.SourceASC->MakeEffectContext()
			Data.Target.ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
	}
}

void UAuraAttributeSet::HandleIncomingXP(const FGameplayEffectModCallbackData& Data, const FEffectProperties& Props)
{	// in GE_EventBased Attributes.Meta.IncomingXP set to "Override"
	AAuraPlayerState* AuraPS = Cast<AAuraPlayerState>(GetOuter());
	if (!AuraPS) return;
	const int32 OldLevel = AuraPS->GetPlayerLevel();
	AuraPS->AddToXP(GetIncomingXP());
	if (OldLevel < AuraPS->GetPlayerLevel())
	{
		const FGameplayEffectSpecHandle SpecHandle = Data.Target.MakeOutgoingSpec(
			UVitalsResetEffect::StaticClass(), 1.f, Data.Target.MakeEffectContext());
		Data.Target.ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}
