// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/EnemyWidgetController.h"
#include "AuraEnemy.generated.h"

class AAuraAIController;
class UBehaviorTree;
class UEnemyWidgetController;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();
	virtual void PossessedBy(AController* NewController) override;

	//~ Begin IEnemyInterface
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	//~ End IEnemyInterface

	//~ Combat Interface
	FORCEINLINE virtual int32 GetCharacterLevel() override { return Level; }
	virtual void Die() override;
	virtual void SetCombatTarget_Implementation(AActor* NewTarget) override {CombatTarget = NewTarget;}
	virtual AActor* GetCombatTarget_Implementation() const override {return CombatTarget;}
	//~ End Combat Interface
	
	UFUNCTION()
	void HitReactChanged(const FGameplayTag CallbackTag, int NewCount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float BaseWalkSpeed = 250.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float LifeSpan = 5.f;
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	// No need to replicate because we only check AI-Controlled on the server
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
	
	UPROPERTY()
	TObjectPtr<UEnemyWidgetController> HealthBarController;
	UPROPERTY(EditAnywhere, Category = "AuraEnemy|AttributeMenu")
	TSubclassOf<UEnemyWidgetController> HealthBarControllerClass;

	// UPROPERTY(EditAnywhere, Category="AI")
	// TObjectPtr<UBehaviorTree> BehaviorTree;
	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;
};
