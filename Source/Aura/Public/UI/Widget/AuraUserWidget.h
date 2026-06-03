// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h" // Add "CommonUI" Build.cs
#include "AuraUserWidget.generated.h"

/**
 * Base class for this project's UserWidget
 */
UCLASS()
class AURA_API UAuraUserWidget : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	/**
	 * - Each Widget will have a Controller to receive delegate, might not be used and opted for Event Parameter instead
	 * - Call Super:: last so that BP_SetWidgetController is called later
	 */
	UFUNCTION(BlueprintCallable)
	virtual void SetWidgetController(class UAuraWidgetController* InWidgetController)
	{
		BP_SetWidgetController(InWidgetController); // Call event in blueprint
	}

	/**
	 * For CreateDragDropOperation Offset. Get Offset so that Player will see mouse at the Pivot Offset
	 * @param MyGeometry
	 * @param Offset: % (0, 0) top-left (1, 1) bot-right. Default (.75, .75) so that mouse doesn't cover the Dragging Item
	 */
	UFUNCTION(BlueprintPure)
	FVector2D GetOffsetToPivot(const FGeometry& MyGeometry, const FVector2D Offset=FVector2D(.75, .75)) const;
protected:
	/** Bind changed event to widget in BP */
	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetWidgetController(UAuraWidgetController* Controller);

	UFUNCTION(BlueprintCallable)
	static void SetSizeBoxWidthHeight(class USizeBox* InSizeBox, const FVector2D& InWidthHeight);
};
