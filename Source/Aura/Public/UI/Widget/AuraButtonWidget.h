// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AuraUserWidget.h"
#include "AuraButtonWidget.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UAuraButtonWidget : public UAuraUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ButtonWidgetClicked();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aura|Properties")
	FVector2D ButtonWidthHeight = FVector2D(40., 40.);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aura|Properties")
	FSlateBrush BorderBrush = FSlateBrush();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aura|Properties")
	FButtonStyle InnerStyle = FButtonStyle();
protected:
	virtual void NativePreConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional))
	TObjectPtr<class USizeBox> SizeBox_Root;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<class UButton> Button;
};
