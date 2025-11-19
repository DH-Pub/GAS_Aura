// Copyright Hung


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "AbilitySystem/Data/LevelUpDataAsset.h"
#include "Character/AuraPlayer.h"
#include "Net/UnrealNetwork.h"

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

void AAuraPlayerState::SetXP(const int32 NewXP)
{
	XP = NewXP;
	int32 i = GetPlayerLevel();
	const int32 NewXPLevel = LevelUpDataAsset->FindLevelForXP(XP);
	if (i != NewXPLevel)
	{
		int32 AttributePointsToAdd = 0;
		int32 SpellPointsToAdd = 0;
		while (i < NewXPLevel)
		{
			const FAuraLevelUpData& LevelUpData = LevelUpDataAsset->LevelUpDataList[++i];
			AttributePointsToAdd += LevelUpData.AttributePointsGain;
			SpellPointsToAdd += LevelUpData.SpellPointsGain;
		}
		AddToAttributePoints(AttributePointsToAdd);
		AddToSpellPoints(SpellPointsToAdd);
		SetLevel(i);
		UAbilityDataAsset::UnlockAbilityByLevel(this, AbilitySystemComponent, Level);
		if (AAuraPlayer* Character = Cast<AAuraPlayer>(GetPawn())) Character->MulticastLevelUpEffects(Level);
	}
	OnXPChangedDelegate.Broadcast(XP);
}
