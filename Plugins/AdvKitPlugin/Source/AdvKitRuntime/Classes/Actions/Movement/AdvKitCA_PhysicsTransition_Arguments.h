// Copyright 2015 Pascal Krabbe

#pragma once

#include "Actions/AdvKitCharacterAction_Arguments.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "AdvKitCA_PhysicsTransition_Arguments.generated.h"

/**
 * @brief Arguments for the UAdvKitCA_PhysicsTransition action. @see UAdvKitCA_PhysicsTransition 
 */
UCLASS(BlueprintType, Blueprintable, hidecategories = Object)
class UAdvKitCA_PhysicsTransition_Arguments : public UAdvKitCharacterAction_Arguments
{
	GENERATED_BODY()
public:

	/** Mode to transition from */
	UPROPERTY(BlueprintReadWrite)
	EAdvKitMovementMode FromMode;

	/** Mode to transition to */
	UPROPERTY(BlueprintReadWrite)
	EAdvKitMovementMode ToMode;

	/** Arguments to give to the subsequent action the physics action redirected to */
	UPROPERTY(BlueprintReadWrite)
	UAdvKitCharacterAction_Arguments* SubArguments;
};
