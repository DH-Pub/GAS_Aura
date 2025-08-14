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
	virtual int32 GetCharacterLevel_Implementation() override {return Level;}
	virtual void Die() override;
	virtual void MulticastHandleDeath_Implementation() override;
	//~ End Combat Interface
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	float LifeSpan = 5.f;
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	// No need to replicate because we only check AI-Controlled on the server
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Default")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	TObjectPtr<UWidgetComponent> HealthBar;
	
	UPROPERTY()
	TObjectPtr<UEnemyWidgetController> HealthBarController;
	UPROPERTY(EditAnywhere, Category="Default")
	TSubclassOf<UEnemyWidgetController> HealthBarControllerClass;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;
};
