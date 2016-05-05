// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/Movement/AdvKitCA_Dodge.h"
#include "Actions/Movement/AdvKitCA_Dodge_Arguments.h"

#include "Player/AdvKitCharacter.h"
#include "Player/AdvKitGravityCharacter.h"
#include "Player/AdvKitOrientationComponent.h"

UAdvKitCA_Dodge::UAdvKitCA_Dodge(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DodgeSpeed = 600;
	DodgeDuration = 0.5f;

	DodgeSpeedCurve.GetRichCurve()->AddKey(0, 1);
	DodgeSpeedCurve.GetRichCurve()->AddKey(1, 1);
}


FAdvKitActionResponse UAdvKitCA_Dodge::Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments, UAdvKitCharacterAction* InterruptedOther)
{
	//Need to know where to go for the dodge
	auto LocalArguments = Cast<UAdvKitCA_Dodge_Arguments>(Arguments);
	if (!IsValid(LocalArguments))
	{
		return Failed();
	}

	//Can only dodge while walking
	if (!MovementComponent->IsWalking())
	{
		return Failed();
	}

	//Already dodging
	if (IsActive())
	{
		return Failed();
	}

	//Dodge needs to be longer than nothing and negative time does not apply here
	if (DodgeDuration <= 0)
	{
		return Failed();
	}

	//Set initital values
	DodgeDirection = LocalArguments->DodgeDirection;
	DodgeAlpha = 0;
	LastCharacterLocation = CharacterOwner->GetActorLocation();
	
	return Super::Start_Implementation(LocalArguments, InterruptedOther);
}

void UAdvKitCA_Dodge::TickComponent(float DeltaSeconds, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

	UAdvKitOrientationComponent* OrienationComponent = nullptr;
	auto GravityOwner = Cast<AAdvKitGravityCharacter>(CharacterOwner);
	if (GravityOwner)
	{
		OrienationComponent = GravityOwner->WorldOrientationComponent;
	}

	//Update dodge 
	DodgeAlpha += DeltaSeconds / DodgeDuration;
	if (DodgeAlpha > 1)
	{
		DodgeAlpha = 1;
		Stop();
		return;
	}

	//If the character is locked to a target, the dodge direction will be modified to accommodate it
	if (CharacterOwner->bLockTarget)
	{
		DodgeDirection = ModifyAccelerationWhenTargetLocked(DodgeDirection);
	}

	//Calculate where to go how fast
	FVector DodgeAcceleration = DodgeDirection.GetSafeNormal()*DodgeSpeed*DodgeSpeedCurve.GetRichCurve()->Eval(DodgeAlpha);

	//Actual movement is governed by delta time
	FVector DodgeDelta = DodgeAcceleration * DeltaSeconds;
	if (OrienationComponent)
	{
		DodgeDelta = OrienationComponent->TransformDirectionW2L(DodgeDelta);
	}

	//Calculate location changes for rotation
	FVector NewCharacterLocation = CharacterOwner->GetActorLocation();
	FVector CharacterLocationDelta = NewCharacterLocation - LastCharacterLocation;
	LastCharacterLocation = NewCharacterLocation;

	if (OrienationComponent)
	{
		CharacterLocationDelta = OrienationComponent->TransformDirectionW2L(CharacterLocationDelta);
	}
	CharacterLocationDelta.Z = 0;

	//In case movement delta is zero rotator cannot be created 
	if (CharacterLocationDelta.IsNearlyZero())
	{
		//Substitute with intended delta
		CharacterLocationDelta = DodgeDelta;
		if (OrienationComponent)
		{
			CharacterLocationDelta = OrienationComponent->TransformDirectionW2L(CharacterLocationDelta);
		}
		CharacterLocationDelta.Z = 0;
	}

	FRotator NewRotation = CharacterOwner->GetActorRotation();
	if (OrienationComponent)
	{
		NewRotation = OrienationComponent->TransformRotationW2L(NewRotation);
	}

	if (!CharacterLocationDelta.IsNearlyZero())
	{
		NewRotation = CharacterLocationDelta.Rotation();
	}

	if (OrienationComponent)
	{
		NewRotation = OrienationComponent->TransformRotationL2W(NewRotation);
		DodgeDelta = OrienationComponent->TransformDirectionL2W(DodgeDelta);
	}

	//Apply movement and rotation
	MovementComponent->AccumulateRootMotion(DodgeDelta, NewRotation);
}

FVector UAdvKitCA_Dodge::ModifyAccelerationWhenTargetLocked(const FVector& OriginalAcceleration) 
{
	//Cannot modify acceleration without target
	if (!CharacterOwner || !CharacterOwner->CurrentTarget || !CharacterOwner->bLockTarget)
	{
		return OriginalAcceleration;
	}

	//Flattened Direction from character to target
	FVector ForwardVector = (CharacterOwner->GetActorLocation() - CharacterOwner->CurrentTarget->GetActorLocation()).GetSafeNormal();
	ForwardVector.Z = 0;

	//Make acceleration vector perpendicular to forward vector
	FVector UpVector = FVector::CrossProduct(ForwardVector, OriginalAcceleration.GetSafeNormal());
	FVector AccelVector = FVector::CrossProduct(UpVector, ForwardVector);

	//Interpolate acceleration vector by difference between intended and actual direction
	float AccelDot = FVector::DotProduct(AccelVector, OriginalAcceleration.GetSafeNormal());
	if (AccelDot < 0.5f)
	{
		return FMath::Lerp<FVector>(OriginalAcceleration, AccelVector*OriginalAcceleration.Size(), AccelDot / 0.5f);
	}

	//Fully commit to radial acceleration
	return AccelVector*OriginalAcceleration.Size();
}

bool UAdvKitCA_Dodge::CanBeInterruptedBy_Implementation(const UAdvKitCharacterAction* Other) const
{
	return false;
}

UAdvKitCharacterAction_Arguments* UAdvKitCA_Dodge::MakeArguments(FVector Direction)
{
	auto Arguments = NewObject<UAdvKitCA_Dodge_Arguments>();
	Arguments->DodgeDirection = Direction;
	return Arguments;
}
