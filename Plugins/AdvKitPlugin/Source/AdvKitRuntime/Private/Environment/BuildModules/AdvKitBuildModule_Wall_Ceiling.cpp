// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"

#include "AdvKitTypes.h"
#include "Player/AdvKitCharacter.h"
#include "Environment/Zones/AdvKitZoneRectangle.h"
#include "Environment/Transitions/AdvKitTransitionComponentPoint.h"
#include "Environment/Transitions/AdvKitTransitionComponentArea.h"
#include "Environment/BuildModules/AdvKitBuildModule_Wall_Ceiling.h"

bool UAdvKitBuildModule_Wall_Ceiling::CanCreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	//Cannot create transitions on non rectangle zone object
	if (!ForZone->IsA(AAdvKitZoneRectangle::StaticClass()))
	{
		return false;
	}

	//Only valid for these movement modes
	if (ForZone->GetPhysics() != EAdvKitMovementMode::ClimbingWall
		&& ForZone->GetPhysics() != EAdvKitMovementMode::ClimbingCeiling)
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
	return MovementProps->bCanClimbWall && MovementProps->bCanClimbCeiling;
}

bool UAdvKitBuildModule_Wall_Ceiling::GatherPotentialTargetZones_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone, TArray<AAdvKitZone*>& OutZones)
{
	OutZones.Empty();

	//Need to know character's movement properties to build transitions
	auto MovementProps = GetCharacterMovementProperties(ForCharacterClass);
	if (!MovementProps)
	{
		return false;
	}

	auto RectZone = Cast<AAdvKitZoneRectangle>(ForZone);

	float YWidth = RectZone->LocalRectMaxBoundary.Y - RectZone->LocalRectMinBoundary.Y;

	FVector LocalBottom = RectZone->LocalRectMinBoundary;
	LocalBottom.Y = FMath::Lerp<float>(LocalBottom.Y,RectZone->LocalRectMaxBoundary.Y,0.5f);

	FVector LocalTop = RectZone->LocalRectMaxBoundary;
	LocalTop.Y = LocalBottom.Y;

	FVector GlobalTop = RectZone->GetTransform().TransformPosition(LocalTop);
	FVector GlobalBottom = RectZone->GetTransform().TransformPosition(LocalBottom);

	float OverlapRadius = YWidth * 2;

	TArray<AAdvKitZone*> TempZones;
	if (!OverlapForCloseZones(RectZone, GlobalBottom, GlobalTop, OverlapRadius, TempZones))
	{
		return false;
	}

	for (auto TargetZone : TempZones)
	{
		//Can only work with rectangle zones
		if (!TargetZone->IsA(AAdvKitZoneRectangle::StaticClass()))
		{
			continue;
		}

		//Can not create a transition from wall to anything but a ceiling
		if (ForZone->GetPhysics() == EAdvKitMovementMode::ClimbingWall && TargetZone->GetPhysics() != EAdvKitMovementMode::ClimbingCeiling)
		{
			continue;
		}

		//Can not create a transition from ceiling to anything but a wall
		if (ForZone->GetPhysics() == EAdvKitMovementMode::ClimbingCeiling && TargetZone->GetPhysics() != EAdvKitMovementMode::ClimbingWall)
		{
			continue;
		}

		OutZones.Add(TargetZone);
	}

	return OutZones.Num() > 0;
}

bool UAdvKitBuildModule_Wall_Ceiling::CreateTransitionBetween_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone)
{
	auto SourceRect = Cast<AAdvKitZoneRectangle>(SourceZone);
	auto TargetRect = Cast<AAdvKitZoneRectangle>(TargetZone);

	if (SourceRect->GetPhysics() == EAdvKitMovementMode::ClimbingWall && TargetRect->GetPhysics() == EAdvKitMovementMode::ClimbingCeiling)
	{
		return CreateTransitionFromWallToCeiling(ForCharacterClass, SourceRect, TargetRect);
	}

	if (SourceRect->GetPhysics() == EAdvKitMovementMode::ClimbingCeiling && TargetRect->GetPhysics() == EAdvKitMovementMode::ClimbingWall)
	{
		return CreateTransitionFromCeilingToWall(ForCharacterClass, SourceRect, TargetRect);
	}

	return false;
}

bool UAdvKitBuildModule_Wall_Ceiling::CreateTransitionFromWallToCeiling(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZoneRectangle* SourceWall, AAdvKitZoneRectangle* TargetCeiling)
{
	//Get config values
	FVector WallCharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, SourceWall);
	FVector CeilingCharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, TargetCeiling);

	//Project zones locations onto each other
	FVector TransitionSourceTop = SourceWall->ConstrainPositionToZone(
		TargetCeiling->ConstrainPositionToZone(TargetCeiling->GetZoneRectMaxWorld(), CeilingCharacterHalfExtent),
		WallCharacterHalfExtent);

	FVector TransitionSourceBottom = SourceWall->ConstrainPositionToZone(
		TargetCeiling->ConstrainPositionToZone(TargetCeiling->GetZoneRectMinWorld(), CeilingCharacterHalfExtent),
		WallCharacterHalfExtent);

	FVector TransitionTargetTop = TargetCeiling->ConstrainPositionToZone(TransitionSourceTop, CeilingCharacterHalfExtent);
	FVector TransitionTargetBottom = TargetCeiling->ConstrainPositionToZone(TransitionSourceBottom, CeilingCharacterHalfExtent);
	
	//Check if transition distance matches a character extent
	float ProjectedTransitionLength = FMath::Sqrt(FMath::Pow(CeilingCharacterHalfExtent.X, 2.0f) + FMath::Pow(WallCharacterHalfExtent.Z, 2.0f));

	float TopDistance = FVector::Dist(TransitionSourceTop, TransitionTargetTop);
	if (FMath::Abs<float>(ProjectedTransitionLength - TopDistance) > ErrorMarginPosition)
	{
		return false;
	}

	float BottomDistance = FVector::Dist(TransitionSourceBottom, TransitionTargetBottom);
	if (FMath::Abs<float>(ProjectedTransitionLength - BottomDistance) > ErrorMarginPosition)
	{
		return false;
	}

	//Check if transition directions match up 
	if (!IsTransitionAligning(TransitionSourceBottom, TransitionTargetBottom, SourceWall->GetActorForwardVector())
		|| !IsTransitionAligning(TransitionSourceTop, TransitionTargetTop, SourceWall->GetActorForwardVector()))
	{
		return false;
	}
	
	//Create transition
	auto Transition = CreateTransitionArea("ToCeiling", SourceWall, TransitionSourceBottom, TransitionTargetBottom, TransitionSourceTop, TransitionTargetTop, ForCharacterClass, TargetCeiling->GetPhysics(), TargetCeiling);
	if (!Transition)
	{
		return false;
	}

	//Since a character is climbing upwards it should use the transition when doing so
	Transition->TransitionDirection = FVector::UpVector;
	return true;
}

bool UAdvKitBuildModule_Wall_Ceiling::CreateTransitionFromCeilingToWall(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZoneRectangle* SourceCeiling, AAdvKitZoneRectangle* TargetWall)
{
	//Get config values
	FVector CeilingCharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, SourceCeiling);
	FVector WallCharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, TargetWall);

	//Project zones locations onto each other
	FVector TransitionTargetTop = TargetWall->ConstrainPositionToZone(
		SourceCeiling->ConstrainPositionToZone(SourceCeiling->GetZoneRectMaxWorld(), CeilingCharacterHalfExtent),
		WallCharacterHalfExtent);

	FVector TransitionTargetBottom = TargetWall->ConstrainPositionToZone(
		SourceCeiling->ConstrainPositionToZone(SourceCeiling->GetZoneRectMinWorld(), CeilingCharacterHalfExtent),
		WallCharacterHalfExtent);

	FVector TransitionSourceTop = SourceCeiling->ConstrainPositionToZone(TransitionTargetTop, CeilingCharacterHalfExtent);
	FVector TransitionSourceBottom = SourceCeiling->ConstrainPositionToZone(TransitionTargetBottom, CeilingCharacterHalfExtent);

	//Check if transition distance matches a character extent
	float ProjectedTransitionLength = FMath::Sqrt(FMath::Pow(CeilingCharacterHalfExtent.X, 2.0f) + FMath::Pow(WallCharacterHalfExtent.Z, 2.0f));

	float TopDistance = FVector::Dist(TransitionSourceTop, TransitionTargetTop);
	if (FMath::Abs<float>(ProjectedTransitionLength - TopDistance) > ErrorMarginPosition)
	{
		return false;
	}

	float BottomDistance = FVector::Dist(TransitionSourceBottom, TransitionTargetBottom);
	if (FMath::Abs<float>(ProjectedTransitionLength - BottomDistance) > ErrorMarginPosition)
	{
		return false;
	}

	//Check if transition directions match up 
	if (!IsTransitionAligning(TransitionSourceBottom, TransitionTargetBottom, -TargetWall->GetActorForwardVector())
		|| !IsTransitionAligning(TransitionSourceTop, TransitionTargetTop, -TargetWall->GetActorForwardVector()))
	{
		return false;
	}

	//Create transition
	auto Transition = CreateTransitionArea("ToWall", SourceCeiling, TransitionSourceBottom, TransitionTargetBottom, TransitionSourceTop, TransitionTargetTop, ForCharacterClass, TargetWall->GetPhysics(), TargetWall);
	if (!Transition)
	{
		return false;
	}

	//Remove horizontal component of transition (since ceiling forward is "down")
	Transition->TransitionDirection.X = 0;
	Transition->TransitionDirection.Normalize();
	
	Transition->MinDirectionNormal = GetCharacterMovementComponent(ForCharacterClass)->CeilingMinTransitionDot;
	return true;
}


bool UAdvKitBuildModule_Wall_Ceiling::IsTransitionAligning(FVector SourceLocation, FVector TargetLocation, FVector ComparisonDirection)
{
	FVector TransitionDirection = TargetLocation - SourceLocation;
	TransitionDirection.Z = 0;
	TransitionDirection.Normalize();

	float Dot = FVector::DotProduct(ComparisonDirection, TransitionDirection);
	return Dot > (1- ErrorMarginDot);
}
