// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

enum class ECharacterClass : uint8;
class UAuraAbilitySystemComponent;
class UNiagaraSystem;
class UAuraWorldUserWidget;
struct FGameplayAttribute;
class UAuraAttributeSet;
class UDamageTextComponent;
class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

/**
 * UAbilitySystemGlobals::GetAbilitySystemComponentFromActor checks for IAbilitySystemInterface
 * 
 */
UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	virtual void Tick(float DeltaSeconds) override;

	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const {return AbilitySystemComponent;}
	UAuraAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	// Call MulticastHandleDeath(), which should be overriden
	virtual void Die() override {OnDeathDelegate.Broadcast(); MulticastHandleDeath();}
	// Handle Ragdoll, physics called in Die
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

	
	/*
	 * Interface start ====================================================================================
	 */
#pragma region Interfaces
	// IAbilitySystemInterface
	// Define in .cpp or we need to #include "AbilitySystem/AuraAbilitySystemComponent.h" in this file
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual ECharacterClass GetCharacterClass_Implementation() override {return CharacterClass;}
	
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	
	virtual UAnimMontage* GetHitReactMontage_Implementation() override {return HitReactMontage;}
	virtual FTaggedMontage GetRandomAttackMontage_Implementation() override;
	
	virtual bool IsDead_Implementation() const override {return bIsDead;}
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Combat")
	bool bIsDead = false;
	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnDeathSignature OnDeathDelegate;
	
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override {return BloodEffect;}
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	
	virtual int32 IncrementMinionCount_Implementation(const int32 Amount = 1) override {return MinionCount += Amount;}
#pragma endregion
	/*
	 * Interface end ========================================================================================
	 */

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	ECharacterClass CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(BlueprintReadWrite, Category="Combat")
	FVector TargetLocation = FVector();
	UPROPERTY(BlueprintReadWrite, Category="Combat")
	bool bTracking = false; // true if Facing Target
	
	UPROPERTY()
	int32 MinionCount = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bHitReacting = false;
	
	UFUNCTION(NetMulticast, Unreliable)
	void ShowDamageNumber(const AController* SourceController, const FVector& HitLocation, const float Damage, const bool bBlocked, const bool bCrit);
	UFUNCTION(BlueprintImplementableEvent, Category="Combat")
	void BP_ShowDamageNumber(FVector Loc, float Damage, bool bBlocked, bool bCrit);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat|Weapon")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;// AvatarActor: Character, Owner: PlayerState (Player) / Character (AI)
	UPROPERTY()
	TObjectPtr<UAuraAttributeSet> AttributeSet;
	// ASC->InitAbility and Set
	virtual void InitAbilityActorInfo() {}
	
	// Add startup abilities (from server)
	void AddCharacterStartupAbilities() const;

	// Dissolve Effects
	void Dissolve();
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* MIDynamic);
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* MIDynamic);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	TObjectPtr<UMaterialInstance> MeshDissolveMI;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	TObjectPtr<UMaterialInstance> WeaponDissolveMI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<USoundBase> DeathSound;
private:
	UPROPERTY(EditAnywhere, Category="GameplayAbility|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	UPROPERTY(EditAnywhere, Category="GameplayAbility|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassives;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow, TitleProperty="{MontageTag} - {SocketTag}"), Category="Combat")
	TArray<FTaggedMontage> AttackMontages;

	
	/*UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;*/
};
