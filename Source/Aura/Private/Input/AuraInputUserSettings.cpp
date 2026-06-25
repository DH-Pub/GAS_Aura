// Copyright Hung


#include "Input/AuraInputUserSettings.h"

void UAuraInputUserSettings::ApplySettings()
{
	Super::ApplySettings();

	// Add any functionality you want to happen when the input settings are applied to the user
	// This is a good place to put a breakpoint in your debugger to see the flow of
	// how input settings are used :)
}


//==================================== PlayerMappableKeyProfile =====================================

void UAuraPlayerMappableKeyProfile::EquipProfile()
{
	Super::EquipProfile();

	// Do anything you may want to when a new key profile is equipped
}

void UAuraPlayerMappableKeyProfile::UnEquipProfile()
{
	Super::UnEquipProfile();

	// Do anything you may want to when a new key profile is unequipped
}
