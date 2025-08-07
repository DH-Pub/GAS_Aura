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

	UFUNCTION(BlueprintPure)
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const {return AbilitySystemComponent;}
	UAuraAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& InTargetLocation);
	
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow, TitleProperty="{MontageTag} - {SocketEnum}"), Category="Combat")
	TArray<FTaggedMontage> AttackMontages;
	UFUNCTION(BlueprintCallable)
	void GetRandomAttackMontage(FTaggedMontage& TaggedMontage);
	UFUNCTION(BlueprintPure)
	void GetTaggedMontageByTag(const FGameplayTag& MontageTag, FTaggedMontage& TaggedMontage);

	UFUNCTION(BlueprintPure)
	FVector GetCombatSocketLocation(const ECombatSocket SocketEnum) const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bHitReacting = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	/*
	 * Interface start ====================================================================================
	 */
#pragma region Interfaces
	// IAbilitySystemInterface
	// Define in .cpp or we need to #include "AbilitySystem/AuraAbilitySystemComponent.h" in this file
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Call MulticastHandleDeath(), which should be overriden
	virtual void Die() override;
	// Handle Ragdoll, physics. Called in Die
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();
	virtual bool IsDead_Implementation() const override {return bIsDead;}
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Combat")
	bool bIsDead = false;
	
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override {return BloodEffect;}
#pragma endregion
	/*
	 * Interface end ========================================================================================
	 */

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	ECharacterClass CharacterClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Combat")
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(BlueprintReadWrite, Category="Combat")
	FVector TargetLocation = FVector();
	UPROPERTY(BlueprintReadWrite, Category="Combat")
	bool bTracking = false; // true if Facing Target
	
	UPROPERTY()
	TArray<TObjectPtr<AAuraCharacterBase>> Summons;
	void AddNewSummon(AAuraCharacterBase* NewSummon) {Summons.AddUnique(NewSummon);}
	void SummonDied(AAuraCharacterBase* DeadSummon) {Summons.RemoveSingleSwap(DeadSummon);}
	
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	TObjectPtr<UMaterialInstance> MeshDissolveMI;
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* MIDynamic);
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

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> SummonedEffect;

	
	/*UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;*/
};
