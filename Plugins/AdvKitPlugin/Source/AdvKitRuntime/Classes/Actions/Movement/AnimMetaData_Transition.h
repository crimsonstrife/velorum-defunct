// Copyright 2015 Pascal Krabbe

#pragma once

#pragma once
#include "Animation/AnimMetaData.h"
#include "AnimMetaData_Transition.generated.h"

UCLASS()
class UAnimMetaData_Transition : public UAnimMetaData
{
	GENERATED_BODY()

public:
	/** Movement offset the character gets form the animation in character space */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition Prediction")
	FVector LocalTranslationOffset;

	/** Rotation offset the character gets form the animation in character space */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition Prediction")
	FRotator LocalRotationOffset;

	/** Use rotation part of transition prediction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition Prediction")
	bool bAbsoluteTransitionCorrection;
};