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
	BindGameplayAttributeToBroadcast(AttributeSet->GetHealthAttribute(), OnHealthChanged);
	BindGameplayAttributeToBroadcast(AttributeSet->GetMaxHealthAttribute(), OnMaxHealthChanged);
	BindGameplayAttributeToBroadcast(AttributeSet->GetManaAttribute(), OnManaChanged);
	BindGameplayAttributeToBroadcast(AttributeSet->GetMaxManaAttribute(), OnMaxManaChanged);

	PlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastXPToUI);
	// Receive broadcast from AuraAbilitySystemComponent
	AbilitySystemComponent->EffectAssetTags.AddLambda([this](const FGameplayTagContainer& AssetTags)
	{
		for (const FGameplayTag& Tag : AssetTags)
		{
			// "A.1".MatchesTag("A") return True, "A".MatchesTag("A.1") return False
			// if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Message"))))
			if (Tag.MatchesTag(MessageTags::Message))
			{
				// MessageWidgetInfoDelegate.Broadcast(*MessageInfo->FindMessageInfoForTag(Tag));
				TArray<FMessageRow*> RowArray;
				MessageDataTable->GetAllRows(TEXT(""), RowArray);
				for (const FMessageRow* Row : RowArray)
				{
					if (Row->MessageTag.MatchesTagExact(Tag)) MessageTableDelegate.Broadcast(*Row);
				}
				// MessageTableDelegate.Broadcast(*GetDataTableRowByTag<FMessageRow>(MessageDataTable, Tag));
				return;
			}
		}
	});
}

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(AttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AttributeSet->GetMaxMana());

	// if AddCharacterStartupAbilities is called on the server before InitAbilityActorInfo on client
	AuraHUD->BroadcastAllActivatableAbilities();
	BroadcastXPToUI(PlayerState->GetPlayerXP(), PlayerState->GetPlayerLevel(), PlayerState->LevelUpDataAsset);
}

void UOverlayWidgetController::BroadcastXPToUI(const int32 XP, const int32 Level, ULevelUpDataAsset* LevelUpDA) const
{
	const int32 PreLevelUpReq = LevelUpDA->LevelUpDataList[Level - 1].LevelUpRequirement;
	const int32 LevelUpReq = LevelUpDA->LevelUpDataList[Level].LevelUpRequirement;
	const float CurrentLevelXP = XP - PreLevelUpReq;
	const float DeltaLevelReq = LevelUpReq - PreLevelUpReq;
	const float CurrentLevelXPPercent = CurrentLevelXP / DeltaLevelReq;
	OnXPPercentChangedDelegate.Broadcast(Level, CurrentLevelXPPercent);
}
