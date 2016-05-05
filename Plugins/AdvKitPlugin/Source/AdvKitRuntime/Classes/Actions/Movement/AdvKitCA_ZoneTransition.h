// Copyright 2015 Pascal Krabbe

#pragma once

#include "Actions/AdvKitCharacterAction.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "AdvKitCA_ZoneTransition.generated.h"

class UAdvKitCA_ZoneTransition_Arguments;

/**
 * @brief Action that makes the character change zones. It can play an animation that syncs with the
 * transition and dynamically interpolate between starting and end locations to make the transition
 * look smooth even if the animation does not line up correctly.
 */
UCLASS(abstract)
class ADVKITRUNTIME_API UAdvKitCA_ZoneTransition : public UAdvKitCharacterAction
{
	GENERATED_BODY()
protected:

	/** Arguments for the current transition */
	UPROPERTY()
	UAdvKitCA_ZoneTransition_Arguments* CurrentArguments;

	///** Last alpha of the location transition curve */
	//float LastLocationAlpha;

	///** Last alpha of the rotation transition curve */
	//float LastRotationAlpha;

	///** Delta translation during the current transition */
	//FVector TranslationDifference;
	//
	///** Delta rotation during the current transition */
	//FRotator RotationDifference;

	/** New location in target zone after the transition*/
	//UPROPERTY()
	//UAdvKitZoneLocation* NextLocationInZone;


	/** Animation Montage to play during the action. */
	UPROPERTY(BlueprintReadOnly)
	UAnimMontage* CurrentAnimMontage;

public:
	/**
	 * Constructor
	 */
	UAdvKitCA_ZoneTransition(const FObjectInitializer& ObjectInitializer);

	/**
	 * @return	The current arguments this action is using.
	 */
	const UAdvKitCA_ZoneTransition_Arguments* GetCurrentArguments() const;


protected:
	UFUNCTION(BlueprintNativeEvent,Category="Animation")
	UAnimMontage* GetMontageToPlay();
	virtual UAnimMontage* GetMontageToPlay_Implementation();

	/** Begin UAdvKitCharacterAction Interface */
	virtual void OnMontageEnded_Implementation(UAnimMontage *EndedMontage, bool bInterrupted) override;
public:
	virtual bool CanBeInterruptedBy_Implementation(const UAdvKitCharacterAction* Other) const override;
	virtual FAdvKitActionResponse Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments = NULL, UAdvKitCharacterAction* InterruptedOther = NULL) override;
	/** End UAdvKitCharacterAction Interface */

	/**
	 * Creates an arguments object for this action.
	 * @param	Zone 	Target zone for the transition
	 * @return	Arguments object.
	 */
	static class UAdvKitCharacterAction_Arguments* MakeArguments(AAdvKitZone* Zone);

	/**
	 * Creates an arguments object for this action.
	 * @param	Transition 	Transition to take during the action
	 * @return	Arguments object.
	 */
	static class UAdvKitCharacterAction_Arguments* MakeArguments(UAdvKitTransitionComponent* Transition);
};
