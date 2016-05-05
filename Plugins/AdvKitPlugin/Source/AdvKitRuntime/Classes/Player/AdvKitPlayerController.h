// Copyright 2015 Pascal Krabbe

#pragma once

#include "GameFramework/PlayerController.h"
#include "AdvKitPlayerController.generated.h"

/**
 * @brief This is a slightly modified version of the default player controller and lets
 * the character restrict the view rotation. 
 * 
 * This is useful for first person, when
 * the player cannot turn their head beyond what the skeleton allows for.
 */
UCLASS()
class ADVKITRUNTIME_API AAdvKitPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	/**
	 * Constructor
	 */
	AAdvKitPlayerController(const FObjectInitializer& ObjectInitializer);

	// Begin APlayerController Interface
	virtual void SetControlRotation(const FRotator& NewRotation) override;
	// End APlayerController Interface

protected:

	/** 
	 * Check to make the controller snap roll of the view to zero. 
	 * Some animations might change the camera's roll which causes
	 * problems if it is not used otherwise. 
	 * E.g. the view will tilt and never go back.
	 * 
	 * Note: This is not a nice solution, but I haven't found a 
	 * better alternative so far. May change in the future.
	 */
	UPROPERTY(EditAnywhere)
	bool bUseRollCorrectionHACK;
};
