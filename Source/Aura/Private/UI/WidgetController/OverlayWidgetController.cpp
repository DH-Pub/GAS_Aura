// Copyright Hung


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpDataAsset.h"
#include "Player/AuraPlayerState.h"

/*template<typename T = FTableRowBase> UE_DEPRECATED(all, "just loop through Data Table")
static T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));} // Find by RowName (by FName)*/
void UOverlayWidgetController::BindCallbacksDependencies(UAuraAbilitySystemComponent* InASC)
{
	if (AuraASC)
	{
		AuraASC->AbilityDataDelegate.RemoveAll(this);

		if (const UAuraAttributeSet* OldASC = GetAttributeSet())
		{
			auto RemoveDelegate = [this](const FGameplayAttribute& Attribute)
			{
				AuraASC->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
			};
			RemoveDelegate(OldASC->GetHealthAttribute());
			RemoveDelegate(OldASC->GetMaxHealthAttribute());
			RemoveDelegate(OldASC->GetManaAttribute());
			RemoveDelegate(OldASC->GetMaxManaAttribute());
		}
		if (GetPlayerState()) GetPlayerState()->OnXPChangedDelegate.RemoveAll(this);
	}

	Super::BindCallbacksDependencies(InASC);

	if (!AuraASC) return;
	AuraASC->BindAbilityDataDelegateToUIDelegate(this, OnReceiveAbilityDataFromASC);
	const UAuraAttributeSet* NewAS = GetAttributeSet();
	BindGameplayAttributeToBroadcast(NewAS->GetHealthAttribute(), OnHealthChanged);
	BindGameplayAttributeToBroadcast(NewAS->GetMaxHealthAttribute(), OnMaxHealthChanged);
	BindGameplayAttributeToBroadcast(NewAS->GetManaAttribute(), OnManaChanged);
	BindGameplayAttributeToBroadcast(NewAS->GetMaxManaAttribute(), OnMaxManaChanged);

	if (GetPlayerState()) GetPlayerState()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastXPToUI);
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
	const UAuraAttributeSet* AS = GetAttributeSet();
	OnHealthChanged.Broadcast(AS->GetHealth());
	OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
	OnManaChanged.Broadcast(AS->GetMana());
	OnMaxManaChanged.Broadcast(AS->GetMaxMana());

	// if AddCharacterStartupAbilities is called on the server before Client InitAuraCharacter()->InitAuraHUD
	AuraASC->BroadcastAllAbilityData(); // Make sure Ability Icons on UI receive their data
	BroadcastXPToUI();
}

void UOverlayWidgetController::BroadcastXPToUI(int32 XP) const
{
	if (const AAuraPlayerState* PS = GetPlayerState())
	{
		if (XP == 0) XP = PS->GetPlayerXP();
		const int32 Level = PS->GetPlayerLevel();
		const int32 PrevLevelUpReq = PS->LevelUpDataAsset->LevelUpDataList[Level - 1].LevelUpRequirement;
		const float NextLevelUpReq = PS->LevelUpDataAsset->LevelUpDataList[Level].LevelUpRequirement;
		// have 1 float (NextLevelUpReq) for "/" to return float
		const float CurrentLevelXPPercent = (XP - PrevLevelUpReq) / (NextLevelUpReq - PrevLevelUpReq);
		OnXPPercentChangedDelegate.Broadcast(Level, CurrentLevelXPPercent);
	}
}
