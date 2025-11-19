// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAuraAbilitySystemComponent;
class UAuraAttributeSet;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*Static value*/)
// (Level, CurrentLevelXp, DeltaLevelReq)

/**
 * AuraAbilitySystemComponent and AuraAttributeSet is created in here
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const {return AbilitySystemComponent;}
	UAuraAttributeSet* GetAuraAttributeSet() const { return AttributeSet; }

	// Interfaces ======================================================================================================
	/* IAbilitySystemInterface
	 * Define in .cpp or we need to #include "AbilitySystem/AuraAbilitySystemComponent.h" in this file*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// End Interface ===================================================================================================

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<class ULevelUpDataAsset> LevelUpDataAsset;

	FOnPlayerStatChanged OnLevelChangedDelegate;
	FOnPlayerStatChanged OnXPChangedDelegate;

	FOnPlayerStatChanged OnAttributePointsChangedDelegate;
	FOnPlayerStatChanged OnSpellPointsChangedDelegate;

	FORCEINLINE int32 GetPlayerLevel() const {return Level;}
	void SetLevel(const int32 NewLevel) {Level = NewLevel; OnLevelChangedDelegate.Broadcast(Level);}
	void AddLevel(const int32 PlusLevel) {SetLevel(Level + PlusLevel);}

	FORCEINLINE int32 GetPlayerXP() const {return XP;}
	void SetXP(const int32 NewXP);
	void AddToXP(const int32 PlusXP) {SetXP(XP + PlusXP);}

	FORCEINLINE int32 GetAttributePoints() const {return AttributePoints;}
	void SetAttributePoints(const int32 NewPoints) {AttributePoints = NewPoints; OnAttributePointsChangedDelegate.Broadcast(AttributePoints);}
	void AddToAttributePoints(const int32 InPoints) {SetAttributePoints(AttributePoints + InPoints);}

	FORCEINLINE int32 GetSpellPoints() const {return SpellPoints;}
	void SetSpellPoints(const int32 NewPoints) {SpellPoints = NewPoints; OnSpellPointsChangedDelegate.Broadcast(SpellPoints);}
	void AddToSpellPoints(const int32 InPoints) {SetSpellPoints(SpellPoints + InPoints);}
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAuraAttributeSet> AttributeSet;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level, Category="Default")
	int32 Level = 1;
	UFUNCTION()
	void OnRep_Level(const int32 OldLevel) const {if (Level != OldLevel) OnLevelChangedDelegate.Broadcast(Level);}

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP, Category="Default")
	int32 XP = 0;
	UFUNCTION()
	void OnRep_XP(const int32 OldXP) const {OnXPChangedDelegate.Broadcast(XP);}

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoints, Category="Default")
	int32 AttributePoints = 0;
	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints) const {OnAttributePointsChangedDelegate.Broadcast(AttributePoints);}

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SpellPoints, Category="Default")
	int32 SpellPoints = 0;
	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints) const {OnSpellPointsChangedDelegate.Broadcast(SpellPoints);}
};
