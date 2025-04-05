// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/EnemyWidgetController.h"
#include "AuraEnemy.generated.h"

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

	//~ Begin IEnemyInterface
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	//~ End IEnemyInterface

	//~ Combat Interface
	FORCEINLINE virtual int32 GetCharacterLevel() override { return Level; }
	//~ End Combat Interface
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
};
