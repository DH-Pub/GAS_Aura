// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interface/EnemyInterface.h"
#include "AuraEnemy.generated.h"

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
	virtual int32 GetCharacterLevel_Implementation() const override {return Level;}
	virtual void MulticastHandleDeath_Implementation() override;
	//~ End Combat Interface

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura")
	float LifeSpan = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY()
	TObjectPtr<class AAuraAIController> AuraAIController;
protected:
	virtual void BeginPlay() override;
	virtual void InitAuraCharacter() override;

	// No need to replicate because we only check AI-Controlled on the server
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura")
	int32 Level = 1;

	UPROPERTY()
	TObjectPtr<class UEnemyWidgetController> HealthBarController;
};
