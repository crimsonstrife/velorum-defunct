// Copyright 2015 Pascal Krabbe

#pragma once

#include "Actions/Movement/AdvKitCA_ZoneTransition.h"
#include "Actions/AnimNotifyState_TransitionCorrection.h"
#include "AdvKitCA_ZoneTransition_NoAnim.generated.h"

class UAdvKitCA_Transition_Arguments;

/**
 */
UCLASS(abstract)
class ADVKITRUNTIME_API UAdvKitCA_ZoneTransition_NoAnim : public UAdvKitCA_ZoneTransition
{
	GENERATED_BODY()

protected:
	/** Interpolation curve for the transition prediction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
	FRuntimeFloatCurve TransitionCurve;

	/** Location before the transition */
	UPROPERTY()
	FTransitionLocation PreviousLocation;
	
	/** New location in target zone after the transition*/
	UPROPERTY()
	FTransitionLocation NextLocation;

	/** Current time of the transition curve */
	float CurrentTransitionTime;

	/** Flag to set the zone the character will transition to as the movement base when starting the action. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	bool bSetTargetZoneBaseAtStart;

	/** Set the target zone as the active one of the character at the start of the action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
	bool ApplyZoneOnStart;

	/** Set the target zone as the active one of the character at the end of the action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
	bool ApplyZoneOnStop;


public:
	/**
	 * Constructor
	 */
	UAdvKitCA_ZoneTransition_NoAnim(const FObjectInitializer& ObjectInitializer);

	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaSeconds, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	//End UActorComponent Interface

	//Begin UAdvKitCharacterAction Interface
	virtual FAdvKitActionResponse Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments = NULL, UAdvKitCharacterAction* InterruptedOther = NULL) override;
	virtual FAdvKitActionResponse Stop_Implementation(const UAdvKitCharacterAction* InterruptedBy = NULL) override;
	//End UAdvKitCharacterAction Interface
};
