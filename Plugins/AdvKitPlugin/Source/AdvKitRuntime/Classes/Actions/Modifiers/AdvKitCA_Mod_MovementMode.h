// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "Actions/AdvKitCharacterAction_CharacterModifier.h"
#include "AdvKitCA_Mod_MovementMode.generated.h"

/**
 * @brief Changes the character's movement mode
 */
UCLASS(DisplayName="Modify Movement Mode")
class ADVKITRUNTIME_API UAdvKitCA_Mod_MovementMode : public UAdvKitCharacterAction_CharacterModifier
{
	GENERATED_BODY()

public:

	/** The new movement mode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (EditCondition = "bChangeMovementMode", Tooltip = "The new movement mode."))
	EAdvKitMovementMode MovementMode;

	/** The new custom movement mode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (EditCondition = "bChangeMovementMode", Tooltip = "The new custom movement mode."))
	uint8 CustomMovementMode;

	//Begin UAdvKitCharacterAction_CharacterModifier Interface
	virtual void ApplyToCharacter(class AAdvKitCharacter* ToCharacter, class UAdvKitCharacterMovementComponent* ToMovementComponent);
	//End UAdvKitCharacterAction_CharacterModifier Interface
};
