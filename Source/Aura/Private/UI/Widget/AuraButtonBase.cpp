// Copyright Hung


#include "UI/Widget/AuraButtonBase.h"

void UAuraButtonBase::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);

	if (GetIsEnabled() && IsInteractionEnabled())
	{
		if (ShouldProcessHoverEvent(EHoverEventSource::MouseEvent))
		{
			NativeOnHovered();
		}
	}
}

void UAuraButtonBase::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);

	if (GetIsEnabled() && IsInteractionEnabled())
	{
		if (ShouldProcessHoverEvent(EHoverEventSource::MouseEvent))
		{
			NativeOnUnhovered();
		}
	}
}
