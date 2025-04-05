// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UNavigationSystemV1;
class USplineComponent;
class UAuraAbilitySystemComponent;
class UAuraInputConfig;
class UInputAction;
class IEnemyInterface;
class UInputMappingContext;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override; // Processes player input
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY()
	TObjectPtr<APawn> ControlledPawn;
	UPROPERTY(EditDefaultsOnly, Category = "Inputs")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditDefaultsOnly, Category = "Inputs|Actions")
	TObjectPtr<UInputAction> MoveAction;
	void Move(const FInputActionValue& InputActionValue);

	UPROPERTY(EditDefaultsOnly, Category = "Inputs|Actions")
	TObjectPtr<UInputAction> ShiftAction;
	void ShiftPress() { bShiftKeyDown = true; }
	void ShiftReleased() { bShiftKeyDown = false; }
	bool bShiftKeyDown = false;

	FHitResult CursorHitResult;
	void CursorTrace();
	TScriptInterface<IEnemyInterface> LastActor;
	TScriptInterface<IEnemyInterface> CurrentActor;


#pragma region AbilitySystem
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	UPROPERTY(EditDefaultsOnly, Category="Inputs")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;
	UAuraAbilitySystemComponent* GetASC();
#pragma endregion


	/**
	 * For multiplayer: Project Settings -> Engine/Navigation System -> Allow Client Side Navigation
	 */
#pragma region ClickMove
	UPROPERTY(EditDefaultsOnly, Category="SplineController")
	bool bDrawNavBox = false;
	UPROPERTY(EditDefaultsOnly, Category="SplineController")
	FVector NavExtent = FVector(300.f, 300.f, 600.f);
	UPROPERTY()
	TObjectPtr<UNavigationSystemV1> NavSystem;

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Inputs")
	float ShortPressThreshold = 0.3f;
	bool bAutoRunning = false;
	bool bTargeting = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "Inputs")
	float AutoRunAcceptanceRadius = 25.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TObjectPtr<USplineComponent> Spline;

	void AutoRun();
#pragma endregion

public:
	// return CursorHitResult
	FORCEINLINE FHitResult GetCursorHitResult() { return CursorHitResult; }
};
