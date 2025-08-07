// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AuraUserWidget.h"
#include "AuraButtonWidget.generated.h"

class USizeBox;
class UBorder;
class UButton;
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties")
	FVector2D ButtonWidthHeight = FVector2D(40., 40.);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties")
	FSlateBrush BorderBrush = FSlateBrush();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties")
	FButtonStyle InnerStyle = FButtonStyle();
protected:
	virtual void NativePreConstruct() override;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UButton> Button;
};
