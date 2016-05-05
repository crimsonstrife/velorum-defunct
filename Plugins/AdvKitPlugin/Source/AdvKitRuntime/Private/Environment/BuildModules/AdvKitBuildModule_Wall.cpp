// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"

#include "AdvKitTypes.h"
#include "Player/AdvKitCharacter.h"
#include "Environment/Zones/AdvKitZoneRectangle.h"
#include "Environment/Transitions/AdvKitTransitionComponentPoint.h"
#include "Environment/Transitions/AdvKitTransitionComponentArea.h"
#include "Environment/BuildModules/AdvKitBuildModule_Wall.h"

bool UAdvKitBuildModule_Wall::CanCreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	//Cannot create transitions on non reactangle zone object
	if (!ForZone->IsA(AAdvKitZoneRectangle::StaticClass()))
	{
		return false;
	}

	//Only valid for this movement mode
	if (ForZone->GetPhysics() != EAdvKitMovementMode::ClimbingWall)
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
	return MovementProps->bCanClimbWall;
}

void UAdvKitBuildModule_Wall::CreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	//Create transitions between walls
	Super::CreateTransitionsFor_Implementation(ForCharacterClass, ForZone);

	//Create transitions to exit wall
	CreateExitTransitions(ForCharacterClass, ForZone);
}

bool UAdvKitBuildModule_Wall::GatherPotentialTargetZones_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone, TArray<AAdvKitZone*>& OutZones)
{
	OutZones.Empty();

	auto MovementProps = GetCharacterMovementProperties(ForCharacterClass);
	if (!MovementProps)
	{
		return false;
	}

	auto RectZone = Cast<AAdvKitZoneRectangle>(ForZone);

	float YWidth = RectZone->LocalRectMaxBoundary.Y - RectZone->LocalRectMinBoundary.Y;

	//Get bottom center point
	FVector LocalBottom = RectZone->LocalRectMinBoundary;
	LocalBottom.Y = FMath::Lerp<float>(LocalBottom.Y,RectZone->LocalRectMaxBoundary.Y,0.5f);

	//Get top center point
	FVector LocalTop = RectZone->LocalRectMaxBoundary;
	LocalTop.Y = LocalBottom.Y;

	FVector GlobalTop = RectZone->GetTransform().TransformPosition(LocalTop);
	FVector GlobalBottom = RectZone->GetTransform().TransformPosition(LocalBottom);

	//Overlap close zones
	float OverlapRadius = YWidth * 2;
	TArray<AAdvKitZone*> TempZones;
	if (!OverlapForCloseZones(RectZone, GlobalBottom, GlobalTop, OverlapRadius, TempZones))
	{
		return false;
	}

	//Filter targets
	for (auto Zone : TempZones)
	{
		//Works only with rectangle zones
		if (!Zone->IsA(AAdvKitZoneRectangle::StaticClass()))
		{
			continue;
		}

		//Works only with this movement mode
		if (Zone->GetPhysics() != EAdvKitMovementMode::ClimbingWall)
		{
			continue;
		}

		OutZones.Add(Zone);
	}

	return OutZones.Num() > 0;
}

bool UAdvKitBuildModule_Wall::CreateTransitionBetween_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone)
{
	//Need to know character's movement properties to build transitions
	auto MovementProps = GetCharacterMovementProperties(ForCharacterClass);
	if (!MovementProps)
	{
		return false;
	}

	FVector CharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, SourceZone);
	FVector OtherCharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, TargetZone);

	auto SourceZoneRect = Cast<AAdvKitZoneRectangle>(SourceZone);
	auto TargetZoneRect = Cast<AAdvKitZoneRectangle>(TargetZone);

	FVector WorldClosestOther = SourceZone->ConstrainPositionToZone(TargetZone->ConstrainPositionToZone(SourceZone->GetActorLocation(), OtherCharacterHalfExtent), CharacterHalfExtent);
	FVector WorldClosestSelf = TargetZone->ConstrainPositionToZone(WorldClosestOther, OtherCharacterHalfExtent);
	WorldClosestOther = SourceZone->ConstrainPositionToZone(WorldClosestSelf, CharacterHalfExtent);

	FVector LocalClosestOther = SourceZone->GetTransform().InverseTransformPositionNoScale(WorldClosestOther);
	FVector LocalClosestSelf = SourceZone->GetTransform().InverseTransformPositionNoScale(WorldClosestSelf);

	FVector ClosestDistance = LocalClosestOther - LocalClosestSelf;

	//Transition has to be on the same Z plane
	if (FMath::Abs<float>(ClosestDistance.Z) > ErrorMarginPosition)
	{
		return false;
	}

	//Wall has to be perpendicular (corner)
	float ForwardDot = FVector::DotProduct(TargetZone->GetActorForwardVector(), SourceZone->GetActorForwardVector());
	if (FMath::Abs<float>(ForwardDot) > ErrorMarginDot)
	{
		return false;
	}

	//Check if the transition is a corner
	FVector TransitionSource = WorldClosestOther;// SourceZone->ConstrainPositionToZone(TargetZone->GetActorLocation(), CharacterHalfExtent);
	FVector TransitionTarget = WorldClosestSelf;// TargetZone->ConstrainPositionToZone(SourceZone->GetActorLocation(), OtherCharacterHalfExtent);
	FVector LocalTransitionSource = SourceZone->GetTransform().InverseTransformPositionNoScale(TransitionSource);

	float PredictedTransitionLengthOuterCorner = FMath::Sqrt(FMath::Pow(CharacterHalfExtent.Y + LocalTransitionSource.X, 2.0f)*2.0f);
	float PredictedTransitionLengthInnerCorner = FMath::Sqrt(FMath::Pow(CharacterHalfExtent.Y, 2.0f)*2.0f);
	float ActualTransitionLength = FVector::Dist(TransitionSource, TransitionTarget);

	FVector SelfCorner = SourceZone->GetPositionInDirection(TransitionSource, SourceZone->ConstrainDirectionToZone(TransitionTarget - TransitionSource, TransitionSource),true,CharacterHalfExtent);
	FVector OtherCorner = TargetZone->GetPositionInDirection(TransitionTarget, TargetZone->ConstrainDirectionToZone(TransitionSource - TransitionTarget, TransitionSource), true, OtherCharacterHalfExtent);
	
	if (
			(FMath::Abs<float>(PredictedTransitionLengthOuterCorner - ActualTransitionLength) > ErrorMarginPosition)
			&&
			(FMath::Abs<float>(PredictedTransitionLengthInnerCorner - ActualTransitionLength) > ErrorMarginPosition)
		)
	{
		return false;
	}

	//Project zones onto each other
	FVector TransitionSourceTop = SourceZoneRect->ConstrainPositionToZone(
		TargetZoneRect->ConstrainPositionToZone(TargetZoneRect->GetZoneRectMaxWorld(), OtherCharacterHalfExtent), 
		CharacterHalfExtent);

	FVector TransitionSourceBottom = SourceZoneRect->ConstrainPositionToZone(
		TargetZoneRect->ConstrainPositionToZone(TargetZoneRect->GetZoneRectMinWorld(), OtherCharacterHalfExtent), 
		CharacterHalfExtent);

	FVector TransitionTargetTop = TargetZoneRect->ConstrainPositionToZone(
		SourceZoneRect->ConstrainPositionToZone(SourceZoneRect->GetZoneRectMaxWorld(), CharacterHalfExtent),
		OtherCharacterHalfExtent);


	FVector TransitionTargetBottom = TargetZoneRect->ConstrainPositionToZone(
		SourceZoneRect->ConstrainPositionToZone(SourceZoneRect->GetZoneRectMinWorld(), CharacterHalfExtent),
		OtherCharacterHalfExtent);

	//Create transition
	auto Transition = CreateTransitionArea("Corner", SourceZone, TransitionSourceBottom, TransitionTargetBottom, TransitionSourceTop, TransitionTargetTop, ForCharacterClass, TargetZone->GetPhysics(), TargetZone);
	if (!Transition)
	{
		return false;
	}

	//Align transition direction to zone, instead of actual transition
	FVector TransitionDirection = Transition->TransitionDirection;
	TransitionDirection.X = 0;
	TransitionDirection.Normalize();
	Transition->TransitionDirection = TransitionDirection;
	return true;
}

void UAdvKitBuildModule_Wall::CreateExitTransitions(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	FVector CharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, ForZone);
	float CharacterRadius = CharacterHalfExtent.X;
	float CharacterHalfHeight = CharacterHalfExtent.Z;

	auto ForRectZone = Cast<AAdvKitZoneRectangle>(ForZone);

	//Transition to platform below
	if (bCreateTransitionToFloor)
	{
		FVector LocalStart1 = ForRectZone->LocalRectMinBoundary;
		FVector LocalStart2 = ForRectZone->LocalRectMinBoundary;
		LocalStart2.Y = ForRectZone->LocalRectMaxBoundary.Y;

		FVector TransitionStart1 = ForRectZone->GetTransform().TransformPositionNoScale(LocalStart1);
		FVector TransitionStart2 = ForRectZone->GetTransform().TransformPositionNoScale(LocalStart2);

		TransitionStart1 = ForRectZone->ConstrainPositionToZone(TransitionStart1, CharacterHalfExtent);
		TransitionStart2 = ForRectZone->ConstrainPositionToZone(TransitionStart2, CharacterHalfExtent);

		FVector TransitionEnd1 = TransitionStart1 + ForRectZone->GetActorForwardVector() * CharacterRadius * 2;
		FVector TransitionEnd2 = TransitionStart2 + ForRectZone->GetActorForwardVector() * CharacterRadius * 2;

	
		auto Transition = CreateTransitionArea("ExitFloor", ForRectZone, TransitionStart1, TransitionEnd1, TransitionStart2, TransitionEnd2, ForCharacterClass, EAdvKitMovementMode::Walking);
		if (Transition)
		{
			Transition->TransitionDirection = -FVector::UpVector;
		}
	}
	
	//Transition to platform above
	if (bCreateTransitionToPlatform)
	{
		FVector LocalStart1 = ForRectZone->LocalRectMaxBoundary;
		FVector LocalStart2 = ForRectZone->LocalRectMaxBoundary;
		LocalStart2.Y = ForRectZone->LocalRectMinBoundary.Y;


		FVector TransitionStart1 = ForRectZone->GetTransform().TransformPositionNoScale(LocalStart1);
		FVector TransitionStart2 = ForRectZone->GetTransform().TransformPositionNoScale(LocalStart2);

		TransitionStart1 = ForRectZone->ConstrainPositionToZone(TransitionStart1, CharacterHalfExtent);
		TransitionStart2 = ForRectZone->ConstrainPositionToZone(TransitionStart2, CharacterHalfExtent);

		FVector TransitionEndOffset = ForRectZone->GetActorUpVector() * CharacterHalfExtent*2
			- ForRectZone->GetActorForwardVector()* CharacterRadius * 3;

		FVector TransitionEnd1 = TransitionStart1 + TransitionEndOffset;
		FVector TransitionEnd2 = TransitionStart2 + TransitionEndOffset;

		auto Transition = CreateTransitionArea("ExitPlatform", ForRectZone, TransitionStart1, TransitionEnd1, TransitionStart2, TransitionEnd2, ForCharacterClass, EAdvKitMovementMode::Walking);
		if (Transition)
		{
			Transition->TransitionDirection = FVector::UpVector;
		}
	}
}
