// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Data/CharacterClassDataAsset.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UNiagaraSystem;
class UAuraWorldUserWidget;
struct FGameplayAttribute;
class UAuraAttributeSet;
class UDamageTextComponent;
class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	// Call MulticastHandleDeath(), which should be overriden
	virtual void Die() override;
	// Handle Ragdoll, physics called in Die
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

	
	/*
	 * Interface start ====================================================================================
	 */
#pragma region Interfaces
	virtual UAnimMontage* GetHitReactMontage_Implementation() override {return HitReactMontage;}
	virtual FTaggedMontage GetRandomAttackMontage_Implementation() override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	virtual bool IsDead_Implementation() const override {return bIsDead;}
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Combat")
	bool bIsDead = false;
	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnDeathSignature OnDeathDelegate;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override {return BloodEffect;}
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual int32 IncrementMinionCount_Implementation(int32 Amount = 1) override;
#pragma endregion
	/*
	 * Interface end ========================================================================================
	 */

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::DefaultClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	TObjectPtr<AActor> CombatTarget;
	UPROPERTY(BlueprintReadWrite, Category="Combat")
	bool bTracking = false;
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
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	// ASC->InitAbility and Set
	virtual void InitAbilityActorInfo() {}
	
	// Add startup abilities (from server)
	void AddCharacterAbilities() const;

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

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow, TitleProperty="{MontageTag} - {SocketTag}"), Category="Combat")
	TArray<FTaggedMontage> AttackMontages;

	
	/*UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;*/
};
