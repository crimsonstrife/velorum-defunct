// Copyright 2015 Pascal Krabbe

#pragma once

#include "Actions/Movement/AdvKitCA_ZoneTransition.h"
#include "AdvKitCA_ZoneTransition_SingleAnim.generated.h"

class UAdvKitCA_Transition_Arguments;

/**
 */
UCLASS(abstract)
class ADVKITRUNTIME_API UAdvKitCA_ZoneTransition_SingleAnim : public UAdvKitCA_ZoneTransition
{
	GENERATED_BODY()

protected:
	/** Montage to play. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transition")
	UAnimMontage* Montage;

public:
	//Begin UAdvKitCA_ZoneTransition Interface
	virtual UAnimMontage* GetMontageToPlay_Implementation() override;
	//End UAdvKitCA_ZoneTransition Interface
};
