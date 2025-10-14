// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interface/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

struct FGameplayEffectSpec;
class UDebuffNiagaraComponent;
class UAuraGameplayAbility;
enum class ECharacterClass : uint8;
class UAuraAbilitySystemComponent;
class UNiagaraSystem;
class UAuraAttributeSet;


/**
 * UAbilitySystemGlobals::GetAbilitySystemComponentFromActor checks for IAbilitySystemInterface
 */
UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	virtual void Tick(float DeltaSeconds) override;

	UDebuffNiagaraComponent* GetDebuffNiagaraComponent() {return BurnDebuffComponent;}
	USkeletalMeshComponent* GetWeapon() {return Weapon;}
	UFUNCTION(BlueprintPure)
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const {return AbilitySystemComponent;}
	UAuraAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Default")
	float BaseWalkSpeed = 250.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Default")
	float BaseYawRate = 540.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Default")
	FRotator BaseRotationRate = FRotator(0., 540., 0.);
	UPROPERTY(BlueprintReadWrite)
	FVector AutoMoveDestination = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow, TitleProperty="{MontageTag} - {SocketEnum}"), Category="Default|Combat")
	TArray<FTaggedMontage> AttackMontages;
	UFUNCTION(BlueprintCallable)
	void GetRandomAttackMontage(FTaggedMontage& TaggedMontage);
	UFUNCTION(BlueprintPure)
	void GetTaggedMontageByTag(const FGameplayTag& MontageTag, FTaggedMontage& TaggedMontage);

	UFUNCTION(BlueprintPure)
	FVector GetCombatSocketLocation(const ECombatSocket SocketEnum);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Default|Combat")
	bool bHitReacting = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default|Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	// Interface start ====================================================================================
#pragma region Interfaces
	// IAbilitySystemInterface
	// Define in .cpp or we need to #include "AbilitySystem/AuraAbilitySystemComponent.h" in this file
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UNiagaraSystem* GetBloodEffect_Implementation() override {return BloodEffect;}
#pragma endregion
	// Interface end ========================================================================================


	UFUNCTION(BlueprintNativeEvent)
	int32 GetCharacterLevel() const;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Default")
	ECharacterClass CharacterClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Combat")
	TObjectPtr<AActor> CombatTarget;
	UPROPERTY(BlueprintReadWrite)
	bool bTracking = false; // true if Facing Target
	UPROPERTY(BlueprintReadOnly)
	FVector AimDirection;

	UPROPERTY(EditAnywhere, Category=Parameter, meta=(ClampMin=0, ClampMax=30, UIMin=0, UIMax=30))
	int32 SummonSpawnThreshold = 2; // When Summons.Num() < this, can summon more
	UPROPERTY()
	TArray<TObjectPtr<AAuraCharacterBase>> Summons;
	void AddNewSummon(AAuraCharacterBase* NewSummon) {Summons.AddUnique(NewSummon);}
	void SummonDied(AAuraCharacterBase* DeadSummon) {Summons.RemoveSingleSwap(DeadSummon);}

#pragma region Death
	// Handle Ragdoll, physics. Called in DeathAbility
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& HitImpulse);
	virtual bool IsDead_Implementation() const override;
#pragma endregion

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Default|Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Default|Combat")
	TObjectPtr<UDebuffNiagaraComponent> BurnDebuffComponent;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;// AvatarActor: Character, Owner: PlayerState (Player) / Character (AI)
	UPROPERTY()
	TObjectPtr<UAuraAttributeSet> AttributeSet;
	// ASC->InitAbility and Set
	virtual void InitAuraCharacter() {}

	// Add startup abilities (from server)
	void AddCharacterStartupAbilities() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Default|Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;

	// Death Effects ==========================================================================================
	void Dissolve();
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* MIDynamic);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Default|Combat")
	TObjectPtr<UMaterialInstance> MeshDissolveMI;
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* MIDynamic);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Default|Combat")
	TObjectPtr<UMaterialInstance> WeaponDissolveMI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Default|Combat")
	TObjectPtr<USoundBase> DeathSound;

	UPROPERTY(EditAnywhere, Category="Default|Abilities")
	TArray<TSubclassOf<UAuraGameplayAbility>> StartupAbilities;
private:
	UPROPERTY(EditDefaultsOnly, Category="Default|Combat")
	TObjectPtr<UNiagaraSystem> SummonedEffect;
};
