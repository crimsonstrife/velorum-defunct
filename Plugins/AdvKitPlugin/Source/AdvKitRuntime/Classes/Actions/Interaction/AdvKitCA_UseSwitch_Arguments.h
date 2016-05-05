// Copyright 2015 Pascal Krabbe

#pragma once

#include "Actions/AdvKitCharacterAction_Arguments.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "AdvKitCA_UseSwitch_Arguments.generated.h"

/**
 * @brief Arguments for the UAdvKitCA_UseSwitch action. @see UAdvKitCA_UseSwitch
 */
UCLASS(BlueprintType, Blueprintable, hidecategories = Object)
class UAdvKitCA_UseSwitch_Arguments : public UAdvKitCharacterAction_Arguments
{
	GENERATED_BODY()
public:

	/** Switch to use (e.g. a lock)*/
	UPROPERTY()
	class AAdvKitUsableSimpleSwitch* UseSwitch;

	/** Item to use switch with (e.g. a key)*/
	UPROPERTY()
	class AAdvKitInventoryItem* UseWithItem;
};
