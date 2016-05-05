// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "Actions/AdvKitCharacterAction_CharacterModifier.h"
#include "AdvKitCA_Mod_Auto_MovementMode.generated.h"

/**
 * @brief Changes the character's movement mode automatically based on the new zone
 */
UCLASS(DisplayName="Apply Transition Movement Mode ")
class ADVKITRUNTIME_API UAdvKitCA_Mod_Auto_MovementMode : public UAdvKitCharacterAction_CharacterModifier
{
	GENERATED_BODY()

public:

	//Begin UAdvKitCharacterAction_CharacterModifier Interface
	virtual void ApplyToCharacter(class AAdvKitCharacter* ToCharacter, class UAdvKitCharacterMovementComponent* ToMovementComponent);
	//End UAdvKitCharacterAction_CharacterModifier Interface
};
