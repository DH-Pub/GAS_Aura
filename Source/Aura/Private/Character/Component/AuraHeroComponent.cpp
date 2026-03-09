// Copyright Hung


#include "Character/Component/AuraHeroComponent.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AuraInputDataAsset.h"
#include "Character/AuraCharacterBase.h"
#include "Input/AuraInputComponent.h"

UAuraHeroComponent::UAuraHeroComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAuraHeroComponent::SetAuraHeroInputComponent(UAuraInputComponent* AuraInputComponent)
{
	AuraInputComponent->BindAbilityActions(InputDataAsset->InputConfig, InputDataAsset->InputMappingContext,
		this, &UAuraHeroComponent::AbilityPressed, &UAuraHeroComponent::AbilityReleased);

	AuraInputComponent->BindAction(InputDataAsset->MoveAction, ETriggerEvent::Triggered, this, &UAuraHeroComponent::Move);
}

void UAuraHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	GetAuraCharacter();
}

AAuraCharacterBase* UAuraHeroComponent::GetAuraCharacter()
{
	if (AuraCharacter) return AuraCharacter;
	AuraCharacter = Cast<AAuraCharacterBase>(GetOwner());
	if (AuraCharacter) AuraASC = AuraCharacter->GetAuraAbilitySystemComponent();
	return AuraCharacter;
}

void UAuraHeroComponent::Move(const FInputActionValue& InputActionValue)
{
	if (GetAuraCharacter() == nullptr) return;
}

void UAuraHeroComponent::AbilityPressed(const uint8 InputID)
{
	if (AuraASC) AuraASC->AbilityInputPressed(InputID);
	else if (GetAuraCharacter()) AuraASC = AuraCharacter->GetAuraAbilitySystemComponent();
}
void UAuraHeroComponent::AbilityReleased(const uint8 InputID)
{
	if (AuraASC) AuraASC->AbilityInputReleased(InputID);
	else if (GetAuraCharacter()) AuraASC = AuraCharacter->GetAuraAbilitySystemComponent();
}
