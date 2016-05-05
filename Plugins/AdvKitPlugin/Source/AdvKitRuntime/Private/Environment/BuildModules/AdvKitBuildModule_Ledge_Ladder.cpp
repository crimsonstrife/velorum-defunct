// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"

#include "AdvKitTypes.h"
#include "Player/AdvKitCharacter.h"
#include "Environment/Zones/AdvKitZoneLine.h"
#include "Environment/BuildModules/AdvKitBuildModule_Ledge_Ladder.h"
#include "Environment/Transitions/AdvKitTransitionComponentPoint.h"
#include "Environment/Transitions/AdvKitTransitionComponentArea.h"

UAdvKitBuildModule_Ledge_Ladder::UAdvKitBuildModule_Ledge_Ladder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LadderZOffset = -30;
}

bool UAdvKitBuildModule_Ledge_Ladder::CanCreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	//Cannot create transitions on non line zone object
	if (!ForZone->IsA(AAdvKitZoneLine::StaticClass()))
	{
		return false;
	}

	//Only valid for these movement modes
	if (ForZone->GetPhysics() != EAdvKitMovementMode::ClimbingLedge
		&& ForZone->GetPhysics() != EAdvKitMovementMode::ClimbingLadder)
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
	return MovementProps->bCanClimbLedge && MovementProps->bCanClimbLadder;
}


bool UAdvKitBuildModule_Ledge_Ladder::GatherPotentialTargetZones_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone, TArray<AAdvKitZone*>& OutZones)
{
	OutZones.Empty();

	//Need to know character's movement properties to build transitions
	auto MovementComp = GetCharacterMovementComponent(ForCharacterClass);
	if (!MovementComp)
	{
		return false;
	}

	auto LineZone = Cast<AAdvKitZoneLine>(ForZone);
	check(LineZone);

	FVector CharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, LineZone);
	TArray<AAdvKitZone*> TempZones;
	if (!OverlapForCloseZones(LineZone, LineZone->GetZoneStartWorld(), LineZone->GetZoneEndWorld(), MovementComp->JumpDistanceLedgeLadderHorizontal, TempZones))
	{
		return false;
	}

	//Filter out overlap results
	for (auto Zone : TempZones)
	{
		if (!Zone->IsA(AAdvKitZoneLine::StaticClass()))
		{
			continue;
		}

		//Cannot create transitions from ladder to anything but ledge
		if (LineZone->GetPhysics()==EAdvKitMovementMode::ClimbingLadder 
			&& Zone->GetPhysics() == EAdvKitMovementMode::ClimbingLedge)
		{
			OutZones.Add(Zone);
			continue;
		}

		//Cannot create transitions from ledge to anything but ladder
		if (LineZone->GetPhysics() == EAdvKitMovementMode::ClimbingLedge
			&& Zone->GetPhysics() == EAdvKitMovementMode::ClimbingLadder)
		{
			OutZones.Add(Zone);
		}
	}

	return OutZones.Num() > 0;
}

bool UAdvKitBuildModule_Ledge_Ladder::CreateTransitionBetween_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone)
{
	if (SourceZone->GetPhysics() == EAdvKitMovementMode::ClimbingLedge)
	{
		return CreateTransitionLedgeToLadder(ForCharacterClass, SourceZone, TargetZone);
	}

	if (SourceZone->GetPhysics() == EAdvKitMovementMode::ClimbingLadder)
	{
		return CreateTransitionLadderToLedge(ForCharacterClass, SourceZone, TargetZone);
	}

	return false;
}

bool UAdvKitBuildModule_Ledge_Ladder::CreateTransitionLedgeToLadder(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* Ledge, AAdvKitZone* Ladder)
{
	auto LedgeLine = Cast<AAdvKitZoneLine>(Ledge);
	auto LadderLine = Cast<AAdvKitZoneLine>(Ladder);

	//Need to know character's movement properties to build transitions
	auto MovementComp = GetCharacterMovementComponent(ForCharacterClass);
	if (!MovementComp)
	{
		return false;
	}

	FVector CharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, Ledge);
	FVector OtherCharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, Ladder);

	//Check if zones are parallel
	float ForwardDot = FVector::DotProduct(Ladder->GetActorForwardVector(), Ledge->GetActorForwardVector());
	if (ForwardDot < (1.0f - ErrorMarginDot))
	{
		return false;
	}

	//Project zones onto each other
	FVector EndLocation = Ladder->ConstrainPositionToZone(Ledge->ConstrainPositionToZone(FVector::ZeroVector, CharacterHalfExtent) + FVector::UpVector*LadderZOffset, OtherCharacterHalfExtent);
	FVector StartLocation = Ledge->ConstrainPositionToZone(EndLocation, CharacterHalfExtent);
	
	FVector AlignedZEnd = FVector(EndLocation.X, EndLocation.Y, StartLocation.Z);

	//Check if zones are actually next to each other
	float DirectionDot = FMath::Abs<float>(FVector::DotProduct(Ledge->GetActorRightVector(), (AlignedZEnd - StartLocation).GetSafeNormal()));
	if (DirectionDot < (1.0f - ErrorMarginDot))
	{
		return false;
	}

	//Check if target zone can be reached by jump
	if (FVector::Dist(StartLocation, AlignedZEnd) > MovementComp->JumpDistanceLedgeLadderHorizontal + ErrorMarginPosition)
	{
		return false;
	}

	auto Transition = CreateTransitionPoint("JumpToLadder", Ledge, StartLocation, EndLocation, ForCharacterClass, Ladder->GetPhysics(), Ladder);
	if (!Transition)
	{
		return false;
	}

	FVector Direction = EndLocation - StartLocation;
	Direction = Direction.ProjectOnTo(Ledge->GetActorRightVector()).GetSafeNormal();
	Transition->TransitionDirection = Ledge->GetTransform().InverseTransformVector(Direction);

	return true;
}

bool UAdvKitBuildModule_Ledge_Ladder::CreateTransitionLadderToLedge(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* Ladder, AAdvKitZone* Ledge)
{
	//Need to know character's movement properties to build transitions
	auto MovementComp = GetCharacterMovementComponent(ForCharacterClass);
	if (!MovementComp)
	{
		return false;
	}

	FVector CharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, Ledge);
	FVector OtherCharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, Ladder);

	//Check if ledge is parallel
	float ForwardDot = FVector::DotProduct(Ledge->GetActorForwardVector(), Ladder->GetActorForwardVector());
	if (ForwardDot < (1.0f - ErrorMarginDot))
	{
		return false;
	}

	//Check if target can be reached by jump
	FVector TargetLocation = Ledge->ConstrainPositionToZone(Ladder->GetActorLocation(), OtherCharacterHalfExtent);
	FVector SourceLocation = Ladder->ConstrainPositionToZone(TargetLocation + FVector::UpVector*LadderZOffset, CharacterHalfExtent);

	FVector AlignedZStart = FVector(SourceLocation.X, SourceLocation.Y, TargetLocation.Z);

	if (FVector::Dist(AlignedZStart, TargetLocation) > MovementComp->JumpDistanceLedgeLadderHorizontal + ErrorMarginPosition)
	{ 
		return false;
	}

	if (!Ladder->IsPointInside(SourceLocation))
	{
		return false;
	}

	auto Transition = CreateTransitionPoint("JumpToLedge", Ladder, SourceLocation, TargetLocation, ForCharacterClass, Ledge->GetPhysics(), Ledge);
	if (!Transition)
	{
		return false;
	}

	FVector Direction = TargetLocation - SourceLocation;
	Direction = Direction.ProjectOnTo(Ladder->GetActorRightVector()).GetSafeNormal();
	Transition->TransitionDirection = Ladder->GetTransform().InverseTransformVector(Direction);

	return true;
}
