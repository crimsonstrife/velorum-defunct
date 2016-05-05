// Copyright 2015 Pascal Krabbe

#pragma once

#include "Actions/Movement/AdvKitCA_ZoneTransition.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "AdvKitCA_ZoneTransition_LocationSwitch.generated.h"

class UAdvKitCA_Transition_Arguments;

/**
 */
UCLASS(abstract)
class ADVKITRUNTIME_API UAdvKitCA_ZoneTransition_LocationSwitch : public UAdvKitCA_ZoneTransition
{
	GENERATED_BODY()

protected:
	/** Montage to play if the character is close to the start of the zone. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transition")
	UAnimMontage* MontageAtStartPoint;

	/** Montage to play if the character is close to the end of the zone. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transition")
	UAnimMontage* MontageAtEndPoint;

public:
	//Begin UAdvKitCA_ZoneTransition Interface
	virtual UAnimMontage* GetMontageToPlay_Implementation() override;
	//End UAdvKitCA_ZoneTransition Interface
};
