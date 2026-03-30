// Copyright Hung


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraTag.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpDataAsset.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

/*template<typename T = FTableRowBase> UE_DEPRECATED(all, "just loop through Data Table")
static T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));} // Find by RowName (by FName)*/
void UOverlayWidgetController::BindCallbacksDependencies()
{
	BindGameplayAttributeToBroadcast(GetAttributeSet()->GetHealthAttribute(), OnHealthChanged);
	BindGameplayAttributeToBroadcast(GetAttributeSet()->GetMaxHealthAttribute(), OnMaxHealthChanged);
	BindGameplayAttributeToBroadcast(GetAttributeSet()->GetManaAttribute(), OnManaChanged);
	BindGameplayAttributeToBroadcast(GetAttributeSet()->GetMaxManaAttribute(), OnMaxManaChanged);

	GetPlayerState()->OnXPChangedDelegate.RemoveAll(this);
	GetPlayerState()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastXPToUI);
	// Receive broadcast from AuraAbilitySystemComponent
	/*GetASC()->EffectAssetTags.RemoveAll(this);
	GetASC()->EffectAssetTags.AddWeakLambda(this, [this](const FGameplayTagContainer& AssetTags)
	{
		TArray<FMessageRow*> RowArray; AuraHUD->MessageDataTable->GetAllRows(/*Whatever#1#TEXT("EffectAssetTags"), RowArray);
		for (const FGameplayTag& Tag : AssetTags)
		{	// MessageTableDelegate.Broadcast(*GetDataTableRowByTag<FMessageRow>(MessageDataTable, Tag));
			if (!Tag.MatchesTag(MessageTags::Message)) continue;
			for (const FMessageRow* Row : RowArray) if (Row->MessageTag.MatchesTagExact(Tag)) {MessageTableDelegate.Broadcast(*Row); return;}
		}
	});*/
}

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAttributeSet()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAttributeSet()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAttributeSet()->GetMana());
	OnMaxManaChanged.Broadcast(GetAttributeSet()->GetMaxMana());

	// if AddCharacterStartupAbilities is called on the server before Client InitAuraCharacter()->InitAuraHUD
	GetASC()->BroadcastAllAbilityData(); // Make sure Ability Icons on UI receive their data
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
