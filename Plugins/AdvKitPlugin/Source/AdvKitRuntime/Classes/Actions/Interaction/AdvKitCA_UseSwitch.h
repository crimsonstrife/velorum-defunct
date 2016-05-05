// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "Actions/AdvKitCharacterActionSingleAnimation.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "AdvKitCA_UseSwitch.generated.h"

/**
 * @brief Action that makes the character use a switch. It positions the character in relation to the switch and then
 * plays the using animation in sync with the switch.
 */
UCLASS(abstract)
class ADVKITRUNTIME_API UAdvKitCA_UseSwitch : public UAdvKitCharacterActionSingleAnimation
{
	GENERATED_BODY()
public:
	/** Switch that is being used (e.g. a lock) */
	UPROPERTY()
	class AAdvKitUsableSimpleSwitch* UseSwitch;

	/** Item the character uses with the switch (e.g. a key) */
	UPROPERTY()
	class AAdvKitInventoryItem* UseWithItem;

	/** Begin UAdvKitCharacterAction Interface */
	virtual FAdvKitActionResponse Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments = NULL, UAdvKitCharacterAction* InterruptedOther = NULL) override;
	/** End UAdvKitCharacterAction Interface */

	/**
	 * Creates an arguments object for this action.
	 * @param	Switch 	The switch to use. (e.g. a lock) 
	 * @param	WithItem 	The item to use with the switch. (e.g. a key)
	 * @return	Arguments for this action.
	 */
	static class UAdvKitCharacterAction_Arguments* MakeArguments(class AAdvKitUsableSimpleSwitch* Switch, class AAdvKitInventoryItem* WithItem);
};
