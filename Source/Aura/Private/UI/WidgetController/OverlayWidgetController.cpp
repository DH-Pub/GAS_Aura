// Copyright Hung


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "AbilitySystem/Data/LevelUpDataAsset.h"
#include "Player/AuraPlayerState.h"
#include "UI/Data/TextDataAsset.h"
#include "UI/HUD/AuraHUD.h"

void UOverlayWidgetController::BindCallbacksDependencies()
{
	BindGameplayAttributeToBroadcast(GetAttributeSet()->GetHealthAttribute(), OnHealthChanged);
	BindGameplayAttributeToBroadcast(GetAttributeSet()->GetMaxHealthAttribute(), OnMaxHealthChanged);
	BindGameplayAttributeToBroadcast(GetAttributeSet()->GetManaAttribute(), OnManaChanged);
	BindGameplayAttributeToBroadcast(GetAttributeSet()->GetMaxManaAttribute(), OnMaxManaChanged);

	GetPlayerState()->OnXPChangedDelegate.RemoveAll(this);
	GetPlayerState()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastXPToUI);
	// Receive broadcast from AuraAbilitySystemComponent
	GetASC()->EffectAssetTags.RemoveAll(this);
	GetASC()->EffectAssetTags.AddLambda([&](const FGameplayTagContainer& AssetTags)
	{
		for (const FGameplayTag& Tag : AssetTags)
		{
			if (!Tag.MatchesTag(MessageTags::Message)) continue;
			// MessageTableDelegate.Broadcast(*GetDataTableRowByTag<FMessageRow>(MessageDataTable, Tag));
			TArray<FMessageRow*> RowArray;
			MessageDataTable->GetAllRows(TEXT("AbilitySystemComponent->EffectAssetTags"), RowArray);
			for (const FMessageRow* Row : RowArray)
			{
				if (Row->MessageTag.MatchesTagExact(Tag)) {MessageTableDelegate.Broadcast(*Row); return;}
			}
		}
	});
}

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAttributeSet()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAttributeSet()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAttributeSet()->GetMana());
	OnMaxManaChanged.Broadcast(GetAttributeSet()->GetMaxMana());

	// if AddCharacterStartupAbilities is called on the server before Client InitAuraCharacter()->InitAuraHUD
	AuraHUD->BroadcastAllActivatableAbilities(); // Make sure Ability Icons on UI receive their data
	BroadcastXPToUI();
}

void UOverlayWidgetController::BroadcastXPToUI(int32 XP) const
{
	if (XP == 0) XP = GetPlayerState()->GetPlayerXP();
	const int32 Level = GetPlayerState()->GetPlayerLevel();
	const int32 PrevLevelUpReq = GetPlayerState()->LevelUpDataAsset->LevelUpDataList[Level - 1].LevelUpRequirement;
	const float NextLevelUpReq = GetPlayerState()->LevelUpDataAsset->LevelUpDataList[Level].LevelUpRequirement;
	// have 1 float (NextLevelUpReq) for "/" to return float
	const float CurrentLevelXPPercent = (XP - PrevLevelUpReq) / (NextLevelUpReq - PrevLevelUpReq);
	OnXPPercentChangedDelegate.Broadcast(Level, CurrentLevelXPPercent);
}
