// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "AdvKitCharacterAction_CharacterModifier.generated.h"

/**
 * @brief Every action can change the state of their respective owner character. This class works as
 * an utility to encapsulate configuration parameters.
 */
UCLASS(Blueprintable, abstract, EditInlineNew, CollapseCategories)
class ADVKITRUNTIME_API UAdvKitCharacterAction_CharacterModifier : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Apply this configuration to a character and movement component.
	 * 
	 * @param	ToCharacter				The character to apply the configuration to.
	 * @param	ToMovementComponent		The movement component to apply the configuration to.
	 */
	virtual void ApplyToCharacter(class AAdvKitCharacter* ToCharacter, class UAdvKitCharacterMovementComponent* ToMovementComponent);
};
