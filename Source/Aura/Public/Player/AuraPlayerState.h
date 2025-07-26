// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

struct FAuraLevelUpData;
class UAuraAttributeSet;
class UAuraAbilitySystemComponent;
class ULevelUpDataAsset;
class UAbilitySystemComponent;
class UAttributeSet;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*Static value*/)
// (Level, CurrentLevelXp, DeltaLevelReq)
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnXPChanged, int32 /*XP*/, int32 /*Level*/, ULevelUpDataAsset* /*Level Up Data Asset*/);

UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	UAuraAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpDataAsset> LevelUpDataAsset;
	
	FOnPlayerStatChanged OnLevelChangedDelegate;
	FOnXPChanged OnXPChangedDelegate;
	FOnPlayerStatChanged OnAttributePointsChangedDelegate;
	FOnPlayerStatChanged OnSpellPointsChangedDelegate;
	
	FORCEINLINE int32 GetPlayerLevel() const {return Level;}
	void SetLevel(const int32 NewLevel) {Level = NewLevel; OnLevelChangedDelegate.Broadcast(Level);}
	void AddLevel(const int32 PlusLevel) {SetLevel(Level + PlusLevel);}
	
	FORCEINLINE int32 GetPlayerXP() const {return XP;}
	void SetXP(const int32 NewXP);
	void AddToXP(const int32 PlusXP) {SetXP(XP + PlusXP);}
	
	FORCEINLINE int32 GetAttributePoints() const {return AttributePoints;}
	void SetAttributePoints(const int32 NewPoints);
	void AddToAttributePoints(const int32 InPoints) {SetAttributePoints(AttributePoints + InPoints);}
	
	FORCEINLINE int32 GetSpellPoints() const {return SpellPoints;}
	void SetSpellPoints(const int32 NewPoints);
	void AddToSpellPoints(const int32 InPoints) {SetAttributePoints(SpellPoints + InPoints);}
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAuraAttributeSet> AttributeSet;
	
private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;
	UFUNCTION()
	void OnRep_Level(int32 OldLevel) const;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP)
	int32 XP = 0;
	UFUNCTION()
	void OnRep_XP(int32 OldXP) const;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoints)
	int32 AttributePoints = 1;
	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints) const
	{
		OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
	};
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SpellPoints)
	int32 SpellPoints = 1;
	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints) const
	{
		OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
	};
};
