// Copyright 2015 Pascal Krabbe

#pragma once

#include "AdvKitPlayerController.h"
#include "AdvKitGravityPlayerController.generated.h"

/**
 * @brief Player controller class for the gravity character.
 */
UCLASS()
class ADVKITRUNTIME_API AAdvKitGravityPlayerController : public AAdvKitPlayerController
{
	GENERATED_BODY()
public:
	// Begin APlayerController Interface
	virtual void UpdateRotation(float DeltaSeconds) override;
	virtual void SetControlRotation(const FRotator& NewRotation) override;
	// End APlayerController Interface

	/**
	 * Helper method to get orientation of the controlled character
	 * @return Orientation component of the character
	 */
	virtual class UAdvKitOrientationComponent* GetOrientationComp();
};
