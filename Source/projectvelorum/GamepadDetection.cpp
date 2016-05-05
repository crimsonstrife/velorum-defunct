// Copyright 2012-2018 Helical Media Productions LLC. All Rights Reserved.

#include "projectvelorum.h"
#include "GamepadDetection.h"

#if PLATFORM_WINDOWS
#define private public
#include "Runtime/Core/Public/Windows/WindowsApplication.h"
#include "Runtime/Core/Private/Windows/XInputInterface.h"
#undef private
#endif

bool UGamepadDetection::XboxControllerConnected()
{
#if PLATFORM_WINDOWS

	if (FSlateApplication::IsInitialized() && FSlateApplication::Get().GetPlatformApplication().IsValid())
	{
		TSharedPtr<FWindowsApplication> app = StaticCastSharedPtr<FWindowsApplication>(FSlateApplication::Get().GetPlatformApplication());
		if (app.IsValid())
		{
			TSharedPtr<XInputInterface> x = app->XInput;
			if (x.IsValid())
			{
				for (int i = 0; i < MAX_NUM_XINPUT_CONTROLLERS; i++)
				{
					if (x->ControllerStates[i].bIsConnected)
						return true;
				}
			}
		}
	}
#endif
	return false;
}
