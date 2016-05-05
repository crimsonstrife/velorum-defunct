// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "Actions/AdvKitCharacterAction.h"
#include "AdvKitCharacterActionSingleAnimation.generated.h"

/**
 * @brief A character action that plays a single animation montage on the character and automatically stops when the animation ends.
 */
UCLASS(abstract, Blueprintable)
class ADVKITRUNTIME_API UAdvKitCharacterActionSingleAnimation : public UAdvKitCharacterAction
{
	GENERATED_BODY()

protected:
	/** Animation Montage to play during the action. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (Tooltip = "Animation Montage to play during the action."))
	UAnimMontage* AnimMontage;

	/** Begin UAdvKitCharacterAction Interface */
	virtual void OnMontageEnded_Implementation(UAnimMontage *EndedMontage, bool bInterrupted) override;

public:
	virtual FAdvKitActionResponse Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments = NULL, UAdvKitCharacterAction* InterruptedOther = NULL) override;
	virtual bool CanBeInterruptedBy_Implementation(const UAdvKitCharacterAction* Other) const override;
	/** End UAdvKitCharacterAction Interface */
};
