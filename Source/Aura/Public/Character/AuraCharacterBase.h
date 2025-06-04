// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Data/CharacterClassDataAsset.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UAuraWorldUserWidget;
struct FGameplayAttribute;
class UAuraAttributeSet;
class UDamageTextComponent;
class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	virtual UAnimMontage* GetHitReactMontage_Implementation() override {return HitReactMontage;}
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::DefaultClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	TObjectPtr<AActor> CombatTarget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bHitReacting = false;

	virtual void Die() override;
	// Handle Ragdoll, physics called in Die
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bIsDead = false;
	
	UFUNCTION(NetMulticast, Unreliable)
	void ShowDamageNumber(const AController* SourceController, const FVector& HitLocation, const float Damage, const bool bBlocked, const bool bCrit);
	UFUNCTION(BlueprintImplementableEvent, Category="Combat")
	void BP_ShowDamageNumber(FVector Loc, float Damage, bool bBlocked, bool bCrit);
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Combat|Weapon")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category="Combat")
	FName WeaponTipSocketName;
	virtual FVector GetCombatSocketLocation() override;

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
	
private:
	UPROPERTY(EditAnywhere, Category="GameplayAbility|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	/*UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;*/
};
