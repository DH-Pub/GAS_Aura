// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "Aura/Aura.h"
#include "Interface/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

enum class EAuraHitDirection : uint8;
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
	explicit AAuraCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	class UAuraMovementComponent* GetAuraMovementComponent() const;

	USkeletalMeshComponent* GetWeapon() {return Weapon;}
	UFUNCTION(BlueprintPure)
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const {return AbilitySystemComponent;}
	UAuraAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura")
	FRotator BaseRotationRate = FRotator(0., 540., 0.);
	UPROPERTY(BlueprintReadWrite)
	FVector AutoMoveDestination = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow, TitleProperty="{MontageTag} - {SocketEnum}"), Category="Aura|Combat")
	TArray<FTaggedMontage> AttackMontages;
	UFUNCTION(BlueprintCallable)
	void GetRandomAttackMontage(FTaggedMontage& TaggedMontage);

	UFUNCTION(BlueprintPure)
	FVector GetCombatSocketLocation(const ECombatSocket SocketEnum) const;
	UFUNCTION(BlueprintPure)
	static FName GetCombatSocketName(const ECombatSocket SocketEnum);
	UFUNCTION(BlueprintPure)
	USceneComponent* GetCombatComponent() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	/** TODO: For educational purpose, Current Project doesn't need this */
	UFUNCTION(/*BlueprintCallable*/)
	EAuraHitDirection GetHitDirection(const FVector& ImpactPoint) const
	{
		const FVector& Loc = GetActorLocation(); // PointPlaneDist is cheap
		const float DistToFrontBack = FVector::PointPlaneDist(ImpactPoint, Loc, GetActorRightVector());
		const float DistToRightLeft = FVector::PointPlaneDist(ImpactPoint, Loc, GetActorForwardVector());

		if (FMath::Abs(DistToFrontBack) < FMath::Abs(DistToRightLeft)) // Determine if Front or Back
		{	// < 0 for opposite from normal vector (like dot product > 90 deg)
			return DistToRightLeft > 0 ? EAuraHitDirection::Front : EAuraHitDirection::Back;
		}
		return DistToFrontBack > 0 ? EAuraHitDirection::Right : EAuraHitDirection::Left;
	}


	// Interface start ====================================================================================
#pragma region Interfaces
	// IAbilitySystemInterface
	// Define in .cpp or we need to #include "AbilitySystem/AuraAbilitySystemComponent.h" in this file
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UNiagaraSystem* GetBloodEffect_Implementation() override {return BloodEffect;}

	virtual bool IsDead_Implementation() const override;
#pragma endregion
	// Interface end ========================================================================================


	UFUNCTION(BlueprintNativeEvent)
	int32 GetCharacterLevel() const;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura")
	ECharacterClass CharacterClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Aura|Combat")
	TObjectPtr<AActor> CombatTarget; // For AI use
	UFUNCTION(BlueprintCallable)
	void SetCombatTarget(AActor* InTarget);
	FTimerHandle TargetCheckTimer;
	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
	float TargetCheckTick = .5f;
	void CheckCombatTarget();
	UPROPERTY()
	bool bTracking = false; // true if Facing Target
	UFUNCTION(BlueprintCallable)
	void SetTracking(const bool bEnable = false);

	UPROPERTY(BlueprintReadOnly, Replicated /* Replicated when changed on server */)
	FVector_NetQuantizeNormal AimDirection;

	UPROPERTY(EditAnywhere, Category=Parameter, meta=(ClampMin=0, ClampMax=30, UIMin=0, UIMax=30))
	int32 SummonSpawnThreshold = 2; // When Summons.Num() < this, can summon more
	UPROPERTY(Replicated)
	TArray<TObjectPtr<AAuraCharacterBase>> Summons;

#pragma region Death
	// Handle Ragdoll, physics. Called in DeathAbility
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& HitImpulse);

	FOnGameplayEffectTagCountChanged& GetOnDeathDelegate() const;
#pragma endregion

protected:
	virtual void BeginPlay() override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;// AvatarActor: Character, Owner: PlayerState (Player) / Character (AI)
	UPROPERTY()
	TObjectPtr<UAuraAttributeSet> AttributeSet;
	// ASC->InitAbility and Set
	virtual void InitAuraCharacter() {}

	// Add startup abilities (from server)
	void AddCharacterStartupAbilities() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;

	// Death Effects ==========================================================================================
	void Dissolve();
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* MIDynamic);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Combat")
	TObjectPtr<UMaterialInstance> MeshDissolveMI;
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* MIDynamic);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Combat")
	TObjectPtr<UMaterialInstance> WeaponDissolveMI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Combat")
	TObjectPtr<USoundBase> DeathSound;

	UPROPERTY(EditAnywhere, Category="Aura|Abilities")
	TArray<TSubclassOf<class UAuraGameplayAbility>> StartupAbilities;
private:
	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
	TObjectPtr<UNiagaraSystem> SummonedEffect;
};
