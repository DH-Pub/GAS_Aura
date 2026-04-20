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

	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<const class ULevelUpDataAsset> LevelUpDataAsset;

	void BroadcastCurrentData() const;
	FOnPlayerStatChanged OnLevelChangedDelegate;
	FOnPlayerStatChanged OnXPChangedDelegate;

	FOnPlayerStatChanged OnAttributePointsChangedDelegate;
	FOnPlayerStatChanged OnSpellPointsChangedDelegate;

	FORCEINLINE int32 GetPlayerLevel() const {return Level;}
	void SetLevel(const int32 NewLevel);
	void AddLevel(const int32 PlusLevel) {SetLevel(Level + PlusLevel);}

	FORCEINLINE int32 GetPlayerXP() const {return XP;}
	void SetXP(const int32 NewXP);
	void AddToXP(const int32 PlusXP) {SetXP(XP + PlusXP);}

	FORCEINLINE int32 GetAttributePoints() const {return AttributePoints;}
	bool SetAttributePoints(const int32 NewPoints)
	{
		if (NewPoints < 0) return false;
		AttributePoints = NewPoints; OnRep_AttributePoints();
		return true;
	} // Call OnRep for Server
	bool AddToAttributePoints(const int32 InPoints) {return SetAttributePoints(AttributePoints + InPoints);}

	FORCEINLINE int32 GetSpellPoints() const {return SpellPoints;}
	void SetSpellPoints(const int32 NewPoints) {SpellPoints = NewPoints; OnRep_SpellPoints();}
	void AddToSpellPoints(const int32 InPoints) {SetSpellPoints(SpellPoints + InPoints);}
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAuraAttributeSet> AttributeSet;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level, Category="Aura")
	int32 Level = 1;
	UFUNCTION()
	void OnRep_Level(int32 OldLevel) const;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP, Category="Aura")
	int32 XP = 0;
	UFUNCTION()
	void OnRep_XP(int32 OldXP) const {OnXPChangedDelegate.Broadcast(XP);}

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_AttributePoints, Category="Aura")
	int32 AttributePoints = 0;
	UFUNCTION()
	void OnRep_AttributePoints() const {OnAttributePointsChangedDelegate.Broadcast(AttributePoints);}

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_SpellPoints, Category="Aura")
	int32 SpellPoints = 0;
	UFUNCTION()
	void OnRep_SpellPoints() const {OnSpellPointsChangedDelegate.Broadcast(SpellPoints);}
};
