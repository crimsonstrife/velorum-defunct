// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Player/AdvKitPlayerController.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "Player/AdvKitCharacter.h"
#include "Player/AdvKitTraceUtilityComponent.h"

#include "Kismet/GameplayStatics.h"

AAdvKitPlayerController::AAdvKitPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseRollCorrectionHACK = true;
}


void AAdvKitPlayerController::SetControlRotation(const FRotator& NewRotation)
{
	AAdvKitCharacter *Character = Cast<AAdvKitCharacter>(GetPawn());
	//No special view modification without a AdvKitCharacter
	if (!Character)
	{
		Super::SetControlRotation(NewRotation);
		return;
	}

	FRotator ModifiedNewRotation = NewRotation;

	if (Character->bLockTarget && Character->TargetLockCamera)
	{
		ModifiedNewRotation = FQuat::Slerp(ControlRotation.Quaternion(),Character->TargetLockCamera->GetComponentQuat(),Character->CameraLerpAlpha).Rotator();
	}

	//Restrict new rotation by character
	Character->RestrictViewRotation(ModifiedNewRotation);

	//TODO hotfix because animations sometimes modify the roll but do not reset it
	if (bUseRollCorrectionHACK)
	{
		ModifiedNewRotation.Roll = 0;
	}

	//Set the new rotation
	Super::SetControlRotation(ModifiedNewRotation);
}

