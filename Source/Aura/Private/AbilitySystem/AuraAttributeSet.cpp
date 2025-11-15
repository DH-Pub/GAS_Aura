// Copyright Hung


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraEffectTypes.h"
#include "AuraGameplayTags.h"
#include "Net/UnrealNetwork.h" // DOREPLIFETIME_CONDITION_NOTIFY
#include "GameplayEffectExtension.h" // FGameplayEffectModCallbackData.EvaluatedData
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Ability/DamageAbility.h"
#include "AbilitySystem/Effect/VitalsResetEffect.h"
#include "Character/AuraCharacterBase.h"
#include "Character/AuraMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"

FEffectProperties::FEffectProperties(const FGameplayEffectModCallbackData& Data)
{
	SourceASC = Data.EffectSpec.GetEffectContext().GetOriginalInstigatorAbilitySystemComponent();
	if (IsValid(SourceASC) && SourceASC->AbilityActorInfo.IsValid() && SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceCharacter = Cast<AAuraCharacterBase>(SourceASC->AbilityActorInfo->AvatarActor.Get());
		SourceController = SourceASC->AbilityActorInfo->PlayerController.Get();
		if (SourceController && SourceCharacter == nullptr) SourceCharacter = Cast<AAuraCharacterBase>(SourceController->GetPawn());
		else if (SourceController == nullptr && SourceCharacter) SourceController = SourceCharacter->GetController();
	}
	
	// Target should be the owner of this AttributeSet
	if (const TSharedPtr<FGameplayAbilityActorInfo> TargetAbilityActorInfo = Data.Target.AbilityActorInfo;
		TargetAbilityActorInfo.IsValid() && TargetAbilityActorInfo->AvatarActor.IsValid())
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
	
	// Meta
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, IncomingDamage, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, IncomingXP, COND_None, REPNOTIFY_Always)
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
	if (Attribute == GetMaxHealthAttribute()) SetHealth(GetHealth()/OldValue * GetMaxHealth());
	else if (Attribute == GetMaxManaAttribute()) SetMana(GetMana()/OldValue * GetMaxMana());
	else if (Attribute == GetMovementSpeedAttribute())
	{
		if (const AAuraCharacterBase* AuraCharacter = Cast<AAuraCharacterBase>(GetActorInfo()->AvatarActor))
		{
			UAuraMovementComponent* MoveComp = static_cast<UAuraMovementComponent*>(AuraCharacter->GetCharacterMovement());
			const float NewSpeed = FMath::Max(NewValue, 0.f);
			MoveComp->MaxWalkSpeed = NewSpeed;
			MoveComp->ClientSetWalkSpeed(NewSpeed);
		}
	}
}


// Called just before BaseValue is changed, can also be used to clamp
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	FEffectProperties Props(Data);
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute()) HandleIncomingDamage(Data, Props);
	else if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute()) HandleIncomingXP(Data, Props);
}

void UAuraAttributeSet::HandleIncomingDamage(const FGameplayEffectModCallbackData& Data, const FEffectProperties& Props)
{
	if (Data.Target.HasMatchingGameplayTag(AuraGameplayTags::Character_State_Death)) return;
	const float LocalIncomingDamage = GetIncomingDamage(); // SetIncomingDamage(0.f); // Old Damage overriden in ExecCalc_Damage
	if (LocalIncomingDamage < UE_KINDA_SMALL_NUMBER) return;
	const float OldHealth = GetHealth();
	SetHealth(FMath::Clamp(OldHealth - LocalIncomingDamage, 0.f, GetMaxHealth()));
	
	FDamageEffectContext* DamageContext = FAuraEffectContext::GetOrMakeContextStructPtr<FDamageEffectContext>(
		Data.EffectSpec.GetContext());
	DamageContext->TargetActor = Props.TargetCharacter;
	if (GetHealth() < UE_KINDA_SMALL_NUMBER)
	{
		FGameplayEventData DeathData; DeathData.ContextHandle = Data.EffectSpec.GetContext();
		Data.Target.HandleGameplayEvent(AuraGameplayTags::Character_State_Death, &DeathData);
	}
	else if (Data.EffectSpec.GetDynamicAssetTags().HasTagExact(AuraGameplayTags::Character_State_HitReact))
	{	// Data.Target.UpdateTagMap(, 1); // AddLooseGameplayTag() SetTagMapCount()
		FGameplayEventData HitData; HitData.ContextHandle = Data.EffectSpec.GetContext();
		Data.Target.HandleGameplayEvent(AuraGameplayTags::Character_State_HitReact, &HitData);
	}
}

void UAuraAttributeSet::HandleIncomingXP(const FGameplayEffectModCallbackData& Data, const FEffectProperties& Props)
{	// in GE_EventBased Attributes.Meta.IncomingXP set to "Override"
	if (AAuraPlayerState* AuraPS = Props.TargetController->GetPlayerState<AAuraPlayerState>())
	{	// Source==Target is the owner, since GA_ListenForEvents applies GE_EventIncomingXP to self
		const int32 OldLevel = AuraPS->GetPlayerLevel();
		AuraPS->AddToXP(GetIncomingXP());
		if (OldLevel < AuraPS->GetPlayerLevel())
		{
			const FGameplayEffectSpecHandle SpecHandle = Data.Target.MakeOutgoingSpec(
				UVitalsResetEffect::StaticClass(), 1.f, Data.Target.MakeEffectContext());
			Data.Target.ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}


// OnRep ---------------------------------------------------------------------------------------------------------------
#pragma region Secondary
void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}
void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}
void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}
void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}
void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}
void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}
#pragma endregion
