// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraButtonWidget.h"
#include "TextButtonWidget.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UTextButtonWidget : public UAuraButtonWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Properties")
	FVector2D ScaleBoxPadding = FVector2D(0.15, 0.15);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Properties")
	FText ButtonText = FText();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Properties")
	FSlateFontInfo FontInfo = FSlateFontInfo();
protected:
	virtual void NativePreConstruct() override;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<class UBorder> Border;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<class UScaleBox> ScaleBox;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<class UTextBlock> Text;
};
