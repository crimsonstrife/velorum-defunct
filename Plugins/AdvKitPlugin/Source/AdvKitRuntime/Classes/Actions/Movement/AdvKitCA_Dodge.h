// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "Actions/AdvKitCharacterAction.h"
#include "AdvKitCA_Dodge.generated.h"

/**
 * @brief Action that makes the character dodge in a given direction.
 */
UCLASS(abstract)
class ADVKITRUNTIME_API UAdvKitCA_Dodge : public UAdvKitCharacterAction
{
	GENERATED_BODY()
public:
	/** This curve handles the percentage of dodge speed at which to dodge at a given time */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FRuntimeFloatCurve DodgeSpeedCurve;

	/** Direction in which to dodge */
	UPROPERTY(BlueprintReadWrite)
	FVector DodgeDirection;

	/** How long to dodge in seconds */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DodgeDuration;

	/** Current progress of the dodging */
	UPROPERTY(BlueprintReadWrite)
	float DodgeAlpha;

	/** How fast to dodge (gets multiplied by the dodge speed curve) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DodgeSpeed;

	/** Location the character had before the current dodge move was executed */
	FVector LastCharacterLocation;

	/**
	 * Constructor
	 */
	UAdvKitCA_Dodge(const FObjectInitializer& ObjectInitializer);

	/**
	 * Modifies movement acceleration if character has a target lock to make it move in a circle around the target.
	 * @param	OriginalAcceleration 	The unmodified acceleration
	 * @return	Modified acceleration that takes the target into account
	 */
	virtual FVector ModifyAccelerationWhenTargetLocked(const FVector& OriginalAcceleration);

	/** Begin UAdvKitCharacterAction Interface */
	virtual FAdvKitActionResponse Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments = NULL, UAdvKitCharacterAction* InterruptedOther = NULL) override;
	virtual bool CanBeInterruptedBy_Implementation(const UAdvKitCharacterAction* Other) const override;
	/** End UAdvKitCharacterAction Interface */

	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaSeconds, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	//End UActorComponent Interface

	/**
	 * Creates an arguments object for the dodge action
	 * @param	Direction 	Direction in which to dodge in world space
	 * @return	Arguments object containing given parameters
	 */
	static class UAdvKitCharacterAction_Arguments* MakeArguments(FVector Direction);
};
