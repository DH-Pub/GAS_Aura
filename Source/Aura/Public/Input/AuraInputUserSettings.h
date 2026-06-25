// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "PlayerMappableKeySettings.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "AuraInputUserSettings.generated.h"

/**
 * Engine - Enhanced Input ->UserSettingsClass
 * Custom settings class for any input related settings for the Aura game.
 * This will be serialized out at the same time as the Aura Shared Settings and is
 * //TODO: Create AuraSharedSettings (Lyra Shared Settings / ULyraSettingsShared)
 * compatible with cloud saves through by calling the "Serialize" function.
 */
UCLASS()
class AURA_API UAuraInputUserSettings : public UEnhancedInputUserSettings
{
	GENERATED_BODY()
public:
	virtual void ApplySettings() override;

	// Add any additional Input Settings here!
	// Some ideas could be:
	// - "toggle vs. hold" to trigger in game actions
	// - aim sensitivity should go here
	// - etc

	// Make sure to mark your properties with the "SaveGame" metadata to have them serialize when saved
	//UPROPERTY(SaveGame, BlueprintReadWrite, Category="Enhanced Input|User Settings")
	// bool bSomeExampleProperty;
};


/**
 * Player Mappable Key settings are settings that are accessible per-action key mapping.
 * This is where you could place additional metadata that may be used by your settings UI,
 * input triggers, or other places where you want to know about a key setting.
 */
UCLASS(MinimalAPI)
class UAuraPlayerMappableKeySettings : public UPlayerMappableKeySettings
{
	GENERATED_BODY()

public:

	/** Returns the tooltip that should be displayed on the settings screen for this key */
	const FText& GetTooltipText() const;

protected:
	/** The tooltip that should be associated with this action when displayed on the settings screen */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta=(AllowPrivateAccess=true))
	FText Tooltip = FText::GetEmpty();
};


/**
 * Engine - Enhanced Input -> DefaultPlayerMappableKeyProfileClass
 */
UCLASS()
class AURA_API UAuraPlayerMappableKeyProfile : public UEnhancedPlayerMappableKeyProfile
{
	GENERATED_BODY()
protected:
	virtual void EquipProfile() override;
	virtual void UnEquipProfile() override;
};
