// Copyright 2015 Pascal Krabbe

#pragma once
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_TransitionCorrection.generated.h"

class UAdvKitCA_ZoneTransition_Arguments;
class UAdvKitCharacterMovementComponent;
class AAdvKitCharacter;

USTRUCT()
struct FTransitionLocation
{
	GENERATED_USTRUCT_BODY();

	/** Location in zone the transition*/
	UPROPERTY()
	UAdvKitZoneLocation* ZoneLocation;

	/** Location if zone location is not valid */
	UPROPERTY()
	FVector Location;

	/** Rotation if zone location is not valid */
	UPROPERTY()
	FRotator Rotation;

	FTransitionLocation()
		: ZoneLocation(nullptr)
		, Location(FVector::ZeroVector)
		, Rotation(FRotator::ZeroRotator)
	{}

	FVector GetLocation(UAdvKitCharacterMovementComponent* MovementComponent) const;
	FRotator GetRotation(UAdvKitCharacterMovementComponent* MovementComponent) const;
	void Clear();
};

UCLASS(meta = (DisplayName = "Transiton Correction"))
class UAnimNotifyState_TransitionCorrection : public UAnimNotifyState
{
	GENERATED_BODY()

protected:

	/** Use rotation part of transition prediction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition Correction")
	bool bAbsoluteTransitionCorrection;

	/** Last alpha of the location transition curve */
	float LastLocationAlpha;

	/** Last alpha of the rotation transition curve */
	float LastRotationAlpha;

	/** Delta translation during the current transition */
	FVector TranslationDifference;

	/** Delta rotation during the current transition */
	FRotator RotationDifference;

	/** Location before the transition */
	UPROPERTY()
	FTransitionLocation PreviousLocation;

	/** New location in target zone after the transition*/
	UPROPERTY()
	FTransitionLocation NextLocation;

	AAdvKitCharacter* GetOwningCharacter(USkeletalMeshComponent * MeshComp) const;
	UAdvKitCharacterMovementComponent* GetOwningMovementComponent(USkeletalMeshComponent * MeshComp) const;
	const UAdvKitCA_ZoneTransition_Arguments* GetCurrentArguments(USkeletalMeshComponent * MeshComp) const;

public:
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;
};