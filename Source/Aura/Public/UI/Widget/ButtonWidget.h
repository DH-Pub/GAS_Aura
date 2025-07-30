// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "UI/Widget/AuraUserWidget.h"
#include "ButtonWidget.generated.h"

class UScaleBox;
class UTextBlock;
class UButton;
class UBorder;
class USizeBox;
/**
 * 
 */
UCLASS()
class AURA_API UButtonWidget : public UAuraUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ButtonWidgetClicked();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	FVector2D ButtonWidthHeight = FVector2D(40., 40.);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	FSlateBrush BorderBrush = FSlateBrush();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	FButtonStyle InnerStyle = FButtonStyle();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	FText ButtonText = FText();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	FSlateFontInfo FontInfo = FSlateFontInfo();
protected:
	virtual void NativePreConstruct() override;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UBorder> Border;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UButton> Button;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UScaleBox> ScaleBox;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> Text;
};
