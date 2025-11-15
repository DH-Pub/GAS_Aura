// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageTextComponent.generated.h"

// Added to AAuraCharacterBase
/*if (DamageTextComponentClass)
	{
		UDamageTextComponent* DmgTxt = NewObject<UDamageTextComponent>(this, DamageTextComponentClass);
		DmgTxt->RegisterComponent();
		// DmgTxt->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform); // to set location
		// DmgTxt->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); // to not when character move
		DmgTxt->SetWorldLocation(GetActorLocation());
		DmgTxt->BP_SetDamageText(Damage);
	}*/
UCLASS()
class UE_DEPRECATED(5.4, "Use AuraWorldUserWidget") AURA_API UDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UDamageTextComponent();
	
	// BlueprintImplementableEvent can't be NetMulticast so we have to call BP function through a NetMulticast function
	UFUNCTION(NetMulticast, Unreliable)
	void SetDamageText(float Damage);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_SetDamageText(float Damage);
};
