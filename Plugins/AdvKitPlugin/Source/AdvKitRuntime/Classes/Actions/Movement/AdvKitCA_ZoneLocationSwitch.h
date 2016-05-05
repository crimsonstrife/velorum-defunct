// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "Actions/AdvKitCharacterAction.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "AdvKitCA_ZoneLocationSwitch.generated.h"

/**
 * @brief Action that plays a different @see UAnimMontage depending on the player being
 * on the left or right side of a wall in a tight space.
 */
UCLASS(abstract, hidecategories = (Animation))
class ADVKITRUNTIME_API UAdvKitCA_ZoneLocationSwitch : public UAdvKitCharacterAction
{
	GENERATED_BODY()
protected:
	/** Action to execute if the character is close to the start of the tight space. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TightSpace")
	TSubclassOf<UAdvKitCharacterAction> ActionAtStartPoint;

	/** Action to execute if the character is close to the end of the tight space. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TightSpace")
	TSubclassOf<UAdvKitCharacterAction> ActionAtEndPoint;


public:
	/** Begin UAdvKitCharacterAction Interface */
	virtual FAdvKitActionResponse Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments = NULL, UAdvKitCharacterAction* InterruptedOther = NULL) override;
	virtual TArray<TSubclassOf<UAdvKitCharacterAction>> GetAdditionalRequiredActions_Implementation() override;
	/** End UAdvKitCharacterAction Interface */
};
