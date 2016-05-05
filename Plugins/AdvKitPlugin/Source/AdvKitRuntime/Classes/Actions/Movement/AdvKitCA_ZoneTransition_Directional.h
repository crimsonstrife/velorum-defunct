// Copyright 2015 Pascal Krabbe

#pragma once

#include "Actions/Movement/AdvKitCA_ZoneTransition.h"
#include "AdvKitCA_ZoneTransition_Directional.generated.h"

class UAdvKitCA_Transition_Arguments;

USTRUCT()
struct FDirectionalMontage
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
	FVector Direction;

	UPROPERTY(EditAnywhere)
	UAnimMontage* Montage;
};

/**
 */
UCLASS(abstract)
class ADVKITRUNTIME_API UAdvKitCA_ZoneTransition_Directional : public UAdvKitCA_ZoneTransition
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
	TArray<FDirectionalMontage> Montages;

public:
	//Begin UAdvKitCA_ZoneTransition Interface
	virtual UAnimMontage* GetMontageToPlay_Implementation() override;
	//End UAdvKitCA_ZoneTransition Interface
};
