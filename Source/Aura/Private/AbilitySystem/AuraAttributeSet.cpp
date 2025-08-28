// Copyright Hung


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "Net/UnrealNetwork.h" // DOREPLIFETIME_CONDITION_NOTIFY
#include "GameplayEffectExtension.h" // FGameplayEffectModCallbackData.EvaluatedData
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Character/AuraCharacterBase.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

FEffectProperties::FEffectProperties(const FGameplayEffectModCallbackData& Data)
{
	EffectContextHandle = Data.EffectSpec.GetContext();
	SourceASC = EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (IsValid(SourceASC) && SourceASC->AbilityActorInfo.IsValid() && SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceAvatarActor = SourceASC->AbilityActorInfo->AvatarActor.Get();
		SourceController = SourceASC->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(SourceAvatarActor))
			{
				SourceController = Pawn->GetController();
			}
		}
		SourceCharacter = Cast<AAuraCharacterBase>(SourceController ? SourceController->GetPawn() : SourceAvatarActor);
	}

	// Target should be the owner of this AttributeSet
	if (const TSharedPtr<FGameplayAbilityActorInfo> TargetAbilityActorInfo = Data.Target.AbilityActorInfo;
		TargetAbilityActorInfo.IsValid() && TargetAbilityActorInfo->AvatarActor.IsValid())
	{
		TargetAvatarActor = TargetAbilityActorInfo->AvatarActor.Get();
		TargetController = TargetAbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<AAuraCharacterBase>(TargetAvatarActor);
		TargetASC = TargetCharacter->GetAuraAbilitySystemComponent();
	}
}

UAuraAttributeSet::UAuraAttributeSet()
{
}

// To Replicate
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

// Each Attribute has BaseValue and CurrentValue
// PreAttributeBaseChange can affect the BaseValue, which affects CurrentValue
// PreAttributeChange can only affect CurrentValue
void UAuraAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

// Called just before BaseValue is changed, can also be used to clamp
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props(Data);
	
	#pragma region IncomingDamage =======================================================================================
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute() && !Props.TargetCharacter->bIsDead)
	{
		const float LocalIncomingDamage = GetIncomingDamage(); // SetIncomingDamage(0.f); // Old Damage overriden in ExecCalc_Damage
		if (LocalIncomingDamage > 0)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
			
			if (NewHealth > 0.f)
			{
				if (UAuraAbilitySystemLibrary::IsStaggerDamage(Props.EffectContextHandle)) // HitReact
				{
					const FGameplayTagContainer TagContainer(AuraGameplayTags::Ability_HitReact); // Container with 1 default
					// Activate GA_HitReact which has AssetTag(Effects.HitReact) given in GiveStartupAbilities(CommonAbilities)
					Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
				}
			}
			else
			{
				Props.TargetCharacter->Die();
				
				// Send XP To Source on death =====================================================================================
				const int32 TargetLevel = ICombatInterface::Execute_GetCharacterLevel(Props.TargetCharacter);
				
				FGameplayEventData Payload;
				Payload.EventTag = AuraGameplayTags::Attributes_Meta_IncomingXP;
				Payload.EventMagnitude = UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(
					Props.TargetCharacter, Props.TargetCharacter->CharacterClass, TargetLevel);
				// GA_ListenForEvent waits to receive
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, Payload.EventTag, Payload); // For last hit player

				Payload.EventMagnitude *= .5f; // For allies
				AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(GetWorld()->GetAuthGameMode());
				for (AAuraPlayerController* Controller : GameMode->PlayerControllers)
				{
					if (Controller == Props.SourceController) continue;
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Controller->GetPawn(), Payload.EventTag, Payload);
				}
			}
			
			// if (Props.SourceCharacter != Props.TargetCharacter)
			const bool bBlocked = UAuraAbilitySystemLibrary::IsBlocked(Props.EffectContextHandle);
			const bool bCrit = UAuraAbilitySystemLibrary::IsCrit(Props.EffectContextHandle);
			FVector HitLoc = Props.TargetAvatarActor->GetActorLocation();
			// use actor's location if not Instant (only popup once), avoid popup in the same place when actor moves
			if (Data.EffectSpec.Def->DurationPolicy == EGameplayEffectDurationType::Instant)
			{
				if (const FHitResult* HitResult = Props.EffectContextHandle.GetHitResult())
				{
					HitLoc = HitResult->ImpactPoint;
				}
			}
			Props.TargetCharacter->ShowDamageNumber(Props.SourceController, HitLoc, LocalIncomingDamage, bBlocked, bCrit);
		}
	}
	#pragma endregion

	
	#pragma region IncomingXP ==========================================================================================
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		const float LocalIncomingXP = GetIncomingXP(); // SetIncomingXP(0.f); // if GE_EventIncomingXP is not "Override"

		// Source/Target is the owner, since GA_ListenForEvents applies GE_EventIncomingXP to self
		if (AAuraPlayerState* AuraPS = Props.TargetController->GetPlayerState<AAuraPlayerState>())
		{
			const int32 OldLevel = AuraPS->GetPlayerLevel();
			AuraPS->AddToXP(LocalIncomingXP);
			if (OldLevel < AuraPS->GetPlayerLevel())
			{
				bTopOfHealth = true;
				bTopOfMana = true;
			}
		}
	}
	#pragma endregion 
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if (Attribute == GetMaxHealthAttribute())
	{
		if (bTopOfHealth)
		{
			SetHealth(GetMaxHealth());
			bTopOfHealth = false;
		}
		else
		{
			const float CurrentHealthPercent = GetHealth()/OldValue;
			SetHealth(CurrentHealthPercent * GetMaxHealth());
		}
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		if (bTopOfMana)
		{
			SetMana(GetMaxMana());
			bTopOfMana = false;
		}
		else
		{
			const float CurrentManaPercent = GetMana()/OldValue;
			SetMana(CurrentManaPercent * GetMaxMana());
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
