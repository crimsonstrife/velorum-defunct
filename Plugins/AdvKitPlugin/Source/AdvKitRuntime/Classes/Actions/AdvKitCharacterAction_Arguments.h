// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"

#include "AdvKitCharacterAction_Arguments.generated.h"

/**
 * @brief Base class to provide argument values to actions.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, hidecategories = Object)
class UAdvKitCharacterAction_Arguments : public UObject
{
	GENERATED_BODY()
public:
	/**
	 * Blueprint hook to create an argument object.
	 * @param	WorldContextObject	Context object of the call
	 * @param	ArgumentClass	Class of argument to instantiate
	 * @return	The new argument object.
	 */
	UFUNCTION(BlueprintPure, Category = "AdvKit|Actions", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static UAdvKitCharacterAction_Arguments* CreateArguments(UObject* WorldContextObject, TSubclassOf<UAdvKitCharacterAction_Arguments> ArgumentClass);
};
