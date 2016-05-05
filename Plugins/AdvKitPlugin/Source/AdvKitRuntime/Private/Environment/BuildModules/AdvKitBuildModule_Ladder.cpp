// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"

#include "AdvKitTypes.h"
#include "Player/AdvKitCharacter.h"
#include "Environment/Zones/AdvKitZoneLine.h"
#include "Environment/Transitions/AdvKitTransitionComponentPoint.h"
#include "Environment/Transitions/AdvKitTransitionComponentArea.h"
#include "Environment/BuildModules/AdvKitBuildModule_Ladder.h"

bool UAdvKitBuildModule_Ladder::CanCreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	//Cannot create transitions on non line zone object
	if (!ForZone->IsA(AAdvKitZoneLine::StaticClass()))
	{
		return false;
	}

	//Only valid for this movement mode
	if (ForZone->GetPhysics() != EAdvKitMovementMode::ClimbingLadder)
	{
		return false;
	}

	//Need to know character's movement properties to build transitions
	auto MovementProps = GetCharacterMovementProperties(ForCharacterClass);
	if (!MovementProps)
	{
		return false;
	}

	//No need to create transitions for a character that cannot use them
	return MovementProps->bCanClimbLadder;
}

void UAdvKitBuildModule_Ladder::CreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	//Create transitions between ladders
	Super::CreateTransitionsFor_Implementation(ForCharacterClass, ForZone);

	//Create transitions to exit ladder
	CreateExitTransitions(ForCharacterClass, ForZone);
}

bool UAdvKitBuildModule_Ladder::GatherPotentialTargetZones_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone, TArray<AAdvKitZone*>& OutZones)
{
	OutZones.Empty();

	//Need to know character's movement properties to build transitions
	auto MovementComp = GetCharacterMovementComponent(ForCharacterClass);
	if (!MovementComp)
	{
		return false;
	}

	auto LineZone = Cast<AAdvKitZoneLine>(ForZone);

	//Get potential target zones
	TArray<AAdvKitZone*> TempZones;
	if (!OverlapForCloseZones(LineZone, LineZone->GetZoneStartWorld(), LineZone->GetZoneEndWorld(), MovementComp->LadderJumpDistanceHorizontal, TempZones))
	{
		return false;
	}

	//Filter targets
	for (auto Zone : TempZones)
	{
		if (Zone->GetPhysics() != EAdvKitMovementMode::ClimbingLadder)
		{
			continue;
		}

		if (!Zone->IsA(AAdvKitZoneLine::StaticClass()))
		{
			continue;
		}

		OutZones.Add(Zone);
	}

	return OutZones.Num() > 0;
}

bool UAdvKitBuildModule_Ladder::CreateTransitionBetween_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone)
{
	//Need to know character's movement properties to build transitions
	auto MovementComp = GetCharacterMovementComponent(ForCharacterClass);
	if (!MovementComp)
	{
		return false;
	}

	FVector CharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, SourceZone);
	FVector OtherCharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, TargetZone);

	auto SourceZoneLine = Cast<AAdvKitZoneLine>(SourceZone);
	auto TargetZoneLine = Cast<AAdvKitZoneLine>(TargetZone);
	
	FVector LocalLocationOther = SourceZoneLine->GetTransform().InverseTransformPositionNoScale(TargetZoneLine->GetActorLocation());

	//Ladder is on the same X plane
	if (FMath::Abs<float>(LocalLocationOther.X) > ErrorMarginPosition)
	{
		return false;
	}

	float ForwardDot = FVector::DotProduct(TargetZoneLine->GetActorForwardVector(), SourceZoneLine->GetActorForwardVector());

	//Ladder is parallel
	if (ForwardDot < (1.0f - ErrorMarginDot))
	{
		return false;
	}

	//Ladder is out of reach
	if (FMath::Abs<float>(LocalLocationOther.Y) > MovementComp->LadderJumpDistanceHorizontal + ErrorMarginPosition)
	{
		return false;
	}

	//Project zone lines onto each other to find overlapping area
	FVector LocalOtherTop = SourceZoneLine->GetTransform().InverseTransformPositionNoScale(TargetZoneLine->ConstrainPositionToZone(TargetZoneLine->GetZoneEndWorld(), OtherCharacterHalfExtent));
	FVector LocalOtherBottom = SourceZoneLine->GetTransform().InverseTransformPositionNoScale(TargetZoneLine->ConstrainPositionToZone(TargetZoneLine->GetZoneStartWorld(), OtherCharacterHalfExtent));

	FVector LocalTop = SourceZoneLine->GetTransform().InverseTransformPositionNoScale(SourceZoneLine->ConstrainPositionToZone(SourceZoneLine->GetZoneEndWorld(), CharacterHalfExtent));
	FVector LocalBottom = SourceZoneLine->GetTransform().InverseTransformPositionNoScale(SourceZoneLine->ConstrainPositionToZone(SourceZoneLine->GetZoneStartWorld(), CharacterHalfExtent));

	float MaxZOverlap = FMath::Min<float>(LocalOtherTop.Z, LocalTop.Z);
	float MinZOverlap = FMath::Max<float>(LocalOtherBottom.Z, LocalBottom.Z);

	FVector StartTop = SourceZoneLine->ConstrainPositionToZone(
		SourceZoneLine->GetTransform().TransformPositionNoScale(FVector(LocalTop.X, LocalTop.Y, MaxZOverlap)),
		CharacterHalfExtent);

	FVector EndTop = TargetZoneLine->ConstrainPositionToZone(
		SourceZoneLine->GetTransform().TransformPositionNoScale(FVector(LocalOtherTop.X, LocalOtherTop.Y, MaxZOverlap)),
		OtherCharacterHalfExtent);

	//Register single transition when overlaps are a single point
	if (MaxZOverlap == MinZOverlap)
	{
		auto Transition = CreateTransitionPoint("SideJump",SourceZoneLine,StartTop, EndTop, ForCharacterClass, TargetZoneLine->GetPhysics(), TargetZoneLine);
		if (Transition)
		{
			Transition->bNeedsJump = bSidewaysTransitionRequiresJump;
			return true;
		}
	}
	//Register transition area
	else if (MaxZOverlap > MinZOverlap)
	{
		FVector StartBottom = SourceZoneLine->ConstrainPositionToZone(
			SourceZoneLine->GetTransform().TransformPositionNoScale(FVector(LocalBottom.X, LocalBottom.Y, MinZOverlap)),
			CharacterHalfExtent);

		FVector EndBottom = TargetZoneLine->ConstrainPositionToZone(
			SourceZoneLine->GetTransform().TransformPositionNoScale(FVector(LocalOtherBottom.X, LocalOtherBottom.Y, MinZOverlap)),
			OtherCharacterHalfExtent);

		auto Transition = CreateTransitionArea("SideJump",SourceZoneLine, StartTop, EndTop, StartBottom, EndBottom, ForCharacterClass, TargetZoneLine->GetPhysics(), TargetZoneLine);
		if (Transition)
		{
			Transition->bNeedsJump = bSidewaysTransitionRequiresJump;
			return true;
		}
	}


	return false;
}

void UAdvKitBuildModule_Ladder::CreateExitTransitions(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	FVector CharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, ForZone);
	float CharacterRadius = CharacterHalfExtent.X;
	float CharacterHalfHeight = CharacterHalfExtent.Z;

	auto ForLineZone = Cast<AAdvKitZoneLine>(ForZone);

	//Transition to platform below
	if (bCreateTransitionToFloor)
	{
		FVector TransitionStart = ForLineZone->ConstrainPositionToZone(ForLineZone->GetZoneStartWorld(), CharacterHalfExtent);
		FVector TransitionEnd = TransitionStart + ForLineZone->GetActorForwardVector() * CharacterRadius * 2;

	
		auto Transition = CreateTransitionPoint("ExitFloor",ForLineZone,TransitionStart, TransitionEnd, ForCharacterClass, EAdvKitMovementMode::Walking);
		if (Transition)
		{
			Transition->TransitionDirection = -FVector::UpVector;
		}
	}
	
	//Transition to platform above
	if (bCreateTransitionToPlatform)
	{
		FVector TransitionStart = ForLineZone->ConstrainPositionToZone(ForLineZone->GetZoneEndWorld(),CharacterHalfExtent);
		
		FVector TransitionEnd = ForLineZone->GetZoneEndWorld() 
			+ ForLineZone->GetActorUpVector() * CharacterHalfExtent 
			- ForLineZone->GetActorForwardVector()* CharacterRadius * 3;

		auto Transition = CreateTransitionPoint("ExitTop",ForLineZone,TransitionStart, TransitionEnd, ForCharacterClass, EAdvKitMovementMode::Walking);
		if (Transition)
		{
			Transition->TransitionDirection = FVector::UpVector;
		}
	}
}
