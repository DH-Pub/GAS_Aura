// Copyright Hung


#include "Player/AuraPlayerState.h"

#include "AuraTag.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "AbilitySystem/Data/LevelUpDataAsset.h"
#include "Character/AuraPlayer.h"
#include "Net/UnrealNetwork.h"
#include "UI/WidgetController/CharacterWidgetController.h"

AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	SetNetUpdateFrequency(120.f); // Update per second
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, XP);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const {return AbilitySystemComponent;}

void AAuraPlayerState::SetLevel(const int32 NewLevel)
{
	const int32 OldLevel = Level;
	Level = NewLevel;
	UAbilityDataAsset::UnlockAbilityByLevel(this, AbilitySystemComponent, Level);
	OnRep_Level(OldLevel);
}

void AAuraPlayerState::SetXP(const int32 NewXP)
{
	const int32 OldXP = XP;
	XP = NewXP;
	int32 OldLv = GetPlayerLevel();
	const int32 NewXPLevel = LevelUpDataAsset->FindLevelForXP(XP);
	if (OldLv != NewXPLevel)
	{
		int32 AttributePointsToAdd = 0;
		int32 SpellPointsToAdd = 0;
		while (OldLv < NewXPLevel)
		{
			const FAuraLevelUpData& LevelUpData = LevelUpDataAsset->LevelUpDataList[++OldLv];
			AttributePointsToAdd += LevelUpData.AttributePointsGain;
			SpellPointsToAdd += LevelUpData.SpellPointsGain;
		}
		AddToAttributePoints(AttributePointsToAdd);
		AddToSpellPoints(SpellPointsToAdd);
		SetLevel(NewXPLevel);
	}

	OnRep_XP(OldXP);
}

void AAuraPlayerState::OnRep_Level(const int32 OldLevel) const
{
	if (Level > OldLevel)
	{
		const APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (PC && GetPawn() && PC->PlayerState == this)
		{	// Cue only on local
			FGameplayCueParameters Params;
			Params.RawMagnitude = Level; // Params.AbilityLevel = NewLevel; Params.GameplayEffectLevel = NewLevel;
			if (GetPawn()) Params.Location = GetPawn()->GetActorLocation();
			Params.Normal = -PC->PlayerCameraManager->GetActorForwardVector();
			Params.Instigator = GetPawn();
			AbilitySystemComponent->InvokeGameplayCueEvent(AuraTag::GameplayCue_Shared_LevelUp,
				EGameplayCueEvent::Executed, Params);

			if (const AAuraPlayer* Chara = Cast<AAuraPlayer>(GetPawn()))
			{	//TODO: use AuraWorldUserWidget
				Chara->CharacterWC->OnLevelUpDelegate.Broadcast(Level);
			}
		}
	}
	OnLevelChangedDelegate.Broadcast(Level);
}
