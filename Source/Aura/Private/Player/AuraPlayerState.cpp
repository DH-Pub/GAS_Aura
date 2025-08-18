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

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const {return AbilitySystemComponent;}

void AAuraPlayerState::SetXP(const int32 NewXP)
{
	checkf(LevelUpDataAsset, TEXT("Unable to find LevelUpData. Please fill out AuraPlayerState"));
	const int32 NewLevel = LevelUpDataAsset->FindLevelForXP(NewXP);
	if (Level != NewLevel)
	{
		AbilitySystemComponent->UpdateAbilityStatuses(NewLevel);
		
		if (AAuraCharacter* Character = Cast<AAuraCharacter>(GetPawn()))
		{
			Character->MulticastLevelUpEffects(NewLevel);
		}
	}
	while (Level < NewLevel)
	{
		Level++;
		const FAuraLevelUpData& LevelUpData = LevelUpDataAsset->LevelUpDataList[Level];
		AddToAttributePoints(LevelUpData.AttributePointsGain);
		AddToSpellPoints(LevelUpData.SpellPointsGain);
	}
	XP = NewXP;
	OnXPChangedDelegate.Broadcast(XP, Level, LevelUpDataAsset);
}
