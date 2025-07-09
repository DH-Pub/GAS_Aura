// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UAuraAttributeSet;
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
	TMap<TObjectPtr<UMaterialInterface>, TObjectPtr<UMaterialInstanceDynamic>> Materials;
	UPROPERTY(BlueprintReadWrite)
	bool IsOccluded = true;
	
	FCameraOccludedStaticMesh(){}
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
	// return CursorHitResult
	FORCEINLINE FHitResult GetCursorHitResult() { return CursorHitResult; }
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

// =========================================================================================================================================
#pragma region Occlusion
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USpringArmComponent> ActiveSpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCameraComponent> ActiveCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCapsuleComponent> CameraCapsule;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="CameraOcclusion")
	TObjectPtr<UMaterialInterface> FadeMaterial;
	UPROPERTY(EditDefaultsOnly, Category="CameraOcclusion")
	float FadeIntensity = .25f;
	UPROPERTY(BlueprintReadWrite, Category="CameraOcclusion")
	TMap<const UStaticMeshComponent*, FCameraOccludedStaticMesh> OccludedMeshes;

	void SetCameraCapsule();
	UFUNCTION(BlueprintCallable)
	void OnCameraCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION(BlueprintCallable)
	void OnCameraCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
#pragma endregion
// ==========================================================================================================================================

	
private:
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

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	FHitResult CursorHitResult;
	void CursorTrace();
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess))
	TScriptInterface<IEnemyInterface> LastActor;
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess))
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


	/*** For multiplayer: Project Settings -> Engine/Navigation System -> Allow Client Side Navigation */
#pragma region ClickMove
	UPROPERTY(EditDefaultsOnly, Category="SplineController")
	bool bDrawNavBox = false;
	UPROPERTY(EditDefaultsOnly, Category="SplineController")
	FVector NavExtent = FVector(300.f, 300.f, 600.f);
	UPROPERTY()
	TObjectPtr<class UNavigationSystemV1> NavSystem;

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Inputs")
	float ShortPressThreshold = 0.3f;
	bool bAutoRunning = false;
	bool bTargeting = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "Inputs")
	float AutoRunAcceptanceRadius = 25.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TObjectPtr<class USplineComponent> Spline;

	void AutoRun();
#pragma endregion
};
