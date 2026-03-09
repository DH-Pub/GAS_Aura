// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AuraUserWidget.h"
#include "AuraWorldUserWidget.generated.h"

/**
 * World to Screen Widget
 * UWidgetBlueprintLibrary::Create
 */
UCLASS()
class AURA_API UAuraWorldUserWidget : public UAuraUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn), Category="UI")
	FVector InitialLocation;

	UPROPERTY(BlueprintReadWrite, Transient, meta=(ExposeOnSpawn), Category="UI")
	TObjectPtr<AActor> AttachedActor;
	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn), Category="UI")
	bool bFollow = true;

	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn), Category="UI")
	FVector2D ScreenOffset = FVector2D(0.f, -10.f);
protected:
	virtual void NativeConstruct() override; // BeginPlay
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	bool bWasOnScreen = true;
private:
	void SetWorldToScreenTranslation(const FVector& WorldLocation);
};
