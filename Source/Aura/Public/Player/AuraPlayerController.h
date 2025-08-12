// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UAuraAbilitySystemComponent;
class UAuraInputConfig;
class UInputAction;
class IEnemyInterface;
class UInputMappingContext;
struct FInputActionValue;

USTRUCT(BlueprintType)
struct FCameraOccludedStaticMesh
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UMaterialInterface>> DefaultMaterials;
	
	/*UPROPERTY(BlueprintReadWrite)
	TMap<TObjectPtr<UMaterialInterface>, TObjectPtr<UMaterialInstanceDynamic>> Materials; // <Default Material, Fade Material>*/
	FCameraOccludedStaticMesh(){}
};

enum EMouseMovementState : uint8
{
	Stop, AutoMove, HoldMove
};
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
	virtual void SetPawn(APawn* InPawn) override;

	UAuraAbilitySystemComponent* GetAuraASC();
	
	UFUNCTION(BlueprintGetter)
	FORCEINLINE FHitResult& GetCursorHitResult() { return CursorHitResult; }

	EMouseMovementState MouseMovementState = Stop;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TObjectPtr<class USplineComponent> Spline;
	FVector AutoMoveDestination = FVector::ZeroVector;
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

// =========================================================================================================================================
#pragma region Occlusion
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCapsuleComponent> CameraCapsule;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Default")
	TObjectPtr<UMaterialInterface> FadeMaterial;
	UPROPERTY(EditDefaultsOnly, Category="Default")
	float FadeIntensity = .25f;
	UPROPERTY()
	TMap<TObjectPtr<UStaticMeshComponent>, FCameraOccludedStaticMesh> OccludedMeshes; // <Mesh, >

	void SetCameraCapsule();
	UFUNCTION()
	void OnCameraCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnCameraCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
#pragma endregion
// ==========================================================================================================================================


private:
	UPROPERTY(EditDefaultsOnly, Category = "Default|Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditDefaultsOnly, Category = "Default|Input")
	TObjectPtr<UInputAction> MoveAction;
	void Move(const FInputActionValue& InputActionValue);

	UPROPERTY(EditDefaultsOnly, Category = "Default|Input")
	TObjectPtr<UInputAction> ShiftAction;
	void ShiftPress() { bShiftKeyDown = true; }
	void ShiftReleased() { bShiftKeyDown = false; }
	bool bShiftKeyDown = false;

	UPROPERTY(BlueprintGetter=GetCursorHitResult, meta=(AllowPrivateAccess))
	FHitResult CursorHitResult;
	void CursorTrace();
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess))
	TScriptInterface<IEnemyInterface> CurrentActor;


#pragma region AbilitySystem
	void ControllerInputPressed(FGameplayTag InputTag);
	void ControllerInputReleased(FGameplayTag InputTag);
	UPROPERTY(EditDefaultsOnly, Category="Default|Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;
#pragma endregion


	/*** For multiplayer: Project Settings -> Engine/Navigation System -> Allow Client Side Navigation */
#pragma region ClickMove
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "Default|Input")
	float AutoRunAcceptanceRadius = 25.f;
#pragma endregion
};
