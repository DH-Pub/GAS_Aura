// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AuraUserWidget.h"
#include "GlobeWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class AURA_API UGlobeWidget : public UAuraUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Properties")
	FSlateBrush RingBrush = FSlateBrush(); // Decorate Ring
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Properties")
	FSlateBrush DefaultBackground = FSlateBrush(); // For Empty Spell Globes
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Properties")
	FMargin InPadding = FMargin(30.f); // Background, Glass, SpellIcon
protected:
	virtual void NativePreConstruct() override;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_Background;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_Glass;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_Ring;
};
