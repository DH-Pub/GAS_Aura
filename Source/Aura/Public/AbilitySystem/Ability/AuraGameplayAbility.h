// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

USTRUCT(BlueprintType)
struct FAbilityDetails
{
	GENERATED_BODY()
	FAbilityDetails(){}
	explicit FAbilityDetails(const int32 Level) : Level(Level){}
	UPROPERTY(BlueprintReadOnly)
	int32 Level = 0;
	UPROPERTY(BlueprintReadOnly)
	float CostMana = 0.f;
	UPROPERTY(BlueprintReadOnly)
	float CostHealth = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float BaseCooldown = 0.f;
	UPROPERTY(BlueprintReadOnly, meta=(Units="Percent"))
	float CooldownReduction = 0.f; // %
	UPROPERTY(BlueprintReadOnly)
	float CalculatedCooldown = 0.f;
};

/**
 * Base GameplayAbility for this project
 * Gameplay Ability is only replicated to the owning player by default
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UAuraGameplayAbility();

	UPROPERTY(EditDefaultsOnly, Category = "Default")
	bool bActivateAbilityOnGranted = false; // Activated on GiveAbility() in OnAvatarSet(), for Passives
protected:
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// Projects may want to initiate passives or do other "BeginPlay" type of logic here.
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	FText AbilityName;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AAuraCharacterBase> AuraCharacter = nullptr;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AAuraPlayerController> AuraPlayerController = nullptr;

	UFUNCTION(BlueprintCallable)
	void EnableMovement(); // This can be called before ability end to enable movement early
	UPROPERTY(EditDefaultsOnly, Category="Default")
	bool bStopMovement = false;
	UPROPERTY(EditDefaultsOnly, Category="Default")
	bool bStopRotation = false;

	FGameplayTagContainer& AddGenericAssetTags(FGameplayTagContainer& Tags);
	void SetBaseCancelBlock();



public:
	UFUNCTION(BlueprintImplementableEvent) // Override this in each ability's BP
	void GetDescription(const FAbilityDetails& Details, FText& OutDescription) const;
	UFUNCTION(BlueprintImplementableEvent) // Override this in each ability's BP
	void GetLevelChangeDescription(const FAbilityDetails& Details, const FAbilityDetails& ChangeDetails,
		FText& OutDescription) const;
	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="Lv"))
	static int32 GetLevelFromDetails(const FAbilityDetails& Details) {return Details.Level;}

};
