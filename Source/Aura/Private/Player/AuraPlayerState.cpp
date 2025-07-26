// Copyright Hung


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpDataAsset.h"
#include "Character/AuraCharacter.h"
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

void AAuraPlayerState::SetXP(const int32 NewXP)
{
	checkf(LevelUpDataAsset, TEXT("Unable to find LevelUpData. Please fill out AuraPlayerState"));
	XP = NewXP;
	const int32 NewLevel = LevelUpDataAsset->FindLevelForXP(XP);
	while (Level < NewLevel)
	{
		Level++;
		if (AAuraCharacter* Character = Cast<AAuraCharacter>(GetPawn()))
		{
			Character->MulticastLevelUpEffects(Level);
		}
		const FAuraLevelUpData& LevelUpData = LevelUpDataAsset->LevelUpDataList[Level];
		// TODO Add rewards
	}
	OnXPChangedDelegate.Broadcast(XP, Level, LevelUpDataAsset);
}

void AAuraPlayerState::SetAttributePoints(const int32 NewPoints)
{
	AttributePoints = NewPoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::SetSpellPoints(const int32 NewPoints)
{
	SpellPoints = NewPoints;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}


#pragma region OnRep
void AAuraPlayerState::OnRep_Level(int32 OldLevel) const
{
	if (Level != OldLevel) OnLevelChangedDelegate.Broadcast(Level);
}
void AAuraPlayerState::OnRep_XP(int32 OldXP) const
{
	OnXPChangedDelegate.Broadcast(XP, Level, LevelUpDataAsset);
}
#pragma endregion