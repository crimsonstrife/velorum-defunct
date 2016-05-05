// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "Actions/AdvKitCharacterAction.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "AdvKitCharacterActionPhysics.generated.h"

/**
 * @brief Character action that can be used to implement a custom movement mode.
 */
UCLASS(abstract)
class ADVKITRUNTIME_API UAdvKitCharacterActionPhysics : public UAdvKitCharacterAction
{
	GENERATED_BODY()
public:
	/**
	 * Constructor
	 */
	UAdvKitCharacterActionPhysics(const FObjectInitializer& ObjectInitializer);

	/**
	 * Called by the movement component during physics to compute translation
	 * @param	DeltaSeconds 	Delta time in seconds since last call
	 */
	virtual void ExecutePhysicsMovement(float DeltaSeconds/*, int32 Iterations*/);

	/**
	 * Called by the movement component during physics to compute rotation
	 * @param	DeltaSeconds 	Delta time in seconds since last call
	 */
	virtual void ExecutePhysicsRotation(float DeltaSeconds);
};
