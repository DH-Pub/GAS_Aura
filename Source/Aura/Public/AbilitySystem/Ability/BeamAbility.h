// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/DamageAbility.h"
#include "Interface/CombatInterface.h"
#include "BeamAbility.generated.h"

class UNiagaraComponent;
namespace EDrawDebugTrace {enum Type : int;}
enum class ECombatSocket : uint8;

/**
 *
 */
UCLASS()
class AURA_API UBeamAbility : public UDamageAbility
{
	GENERATED_BODY()
public:
	UBeamAbility();
protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category="Beam")
	FScalableFloat BeamLength = 600.f;
	UPROPERTY(EditDefaultsOnly, Category="Beam")
	float AngleDeg = 30.f;

	UPROPERTY(EditDefaultsOnly, Category="Beam")
	FScalableFloat MaxChained = 0;

	UFUNCTION(BlueprintCallable)
	void StartBeam(USceneComponent* BeamFromComp, const ECombatSocket StartSocketName);
	FTimerHandle BeamTickTimerHandle; // Apply cost, check if targets are still in range, deals dmg,...
	UPROPERTY(EditDefaultsOnly, Category="Beam")
	float BeamTickInterval = .2f;
	UFUNCTION()
	void BeamTraceForTarget(); // First Beam, handle Chain if hit any enemy
	void ChainBeamOverlaps(int32 ChainBeamRemains, TArray<AActor*>& InActorsToIgnore, const float Length,
		struct FGATargetData_BeamData& BeamData) const;

	UPROPERTY(EditDefaultsOnly, Category="Beam")
	TSubclassOf<class AAuraBeam> BeamClass;
	UPROPERTY(EditDefaultsOnly, Category="Beam")
	TObjectPtr<AAuraBeam> Beam;
private:
	int32 ChainRemains;
protected:
	UPROPERTY(EditDefaultsOnly, Category="Beam|Debug")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebug;
};
