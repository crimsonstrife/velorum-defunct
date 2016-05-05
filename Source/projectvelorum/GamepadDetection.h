// Copyright 2012-2018 Helical Media Productions LLC. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GamepadDetection.generated.h"


/**
*
*/
UCLASS()
class PROJECTVELORUM_API UGamepadDetection : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()public:

		/** Detect if a controller is connected (PC) ! */
		UFUNCTION(BlueprintCallable, Category = "Gamepad")
			static bool XboxControllerConnected();

};
