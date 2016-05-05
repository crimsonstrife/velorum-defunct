// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"

#include "AdvKitTypes.h"
#include "Player/AdvKitCharacter.h"
#include "Environment/Zones/AdvKitZoneLine.h"
#include "Environment/BuildModules/AdvKitBuildModule_Ledge.h"
#include "Environment/Transitions/AdvKitTransitionComponentPoint.h"
#include "Environment/Transitions/AdvKitTransitionComponentArea.h"

UAdvKitBuildModule_Ledge::UAdvKitBuildModule_Ledge(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FloorTraceDistance = 1000;
}


bool UAdvKitBuildModule_Ledge::CanCreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	//Cannot create transitions on non line zone object
	if (!ForZone->IsA(AAdvKitZoneLine::StaticClass()))
	{
		return false;
	}

	//Only valid for this movement mode
	if (ForZone->GetPhysics() != EAdvKitMovementMode::ClimbingLedge)
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
	return MovementProps->bCanClimbLedge;
}


void UAdvKitBuildModule_Ledge::CreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	//Create transitions between ledges
	Super::CreateTransitionsFor_Implementation(ForCharacterClass, ForZone);

	//Create transitions to exit ledge
	CreateExitTransitions(ForCharacterClass, ForZone);
}

bool UAdvKitBuildModule_Ledge::GatherPotentialTargetZones_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone, TArray<AAdvKitZone*>& OutZones)
{
	OutZones.Empty();

	//Need to know character's movement properties to build transitions
	auto MovementComp = GetCharacterMovementComponent(ForCharacterClass);
	if (!MovementComp)
	{
		return false;
	}

	//Can only work with line zones
	auto LineZone = Cast<AAdvKitZoneLine>(ForZone);
	check(LineZone);

	float MaxJumpDistance = FMath::Max<float>(MovementComp->LedgeJumpDistanceHorizontal, MovementComp->LedgeJumpDistanceVertical);

	//Get potential zones
	TArray<AAdvKitZone*> TempZones;
	if (!OverlapForCloseZones(LineZone, LineZone->GetZoneStartWorld(), LineZone->GetZoneEndWorld(), MaxJumpDistance, TempZones))
	{
		return false;
	}

	//Filter out zones that are ledges
	for (auto Zone : TempZones)
	{
		if (Zone->GetPhysics() != EAdvKitMovementMode::ClimbingLedge)
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

bool UAdvKitBuildModule_Ledge::CreateTransitionBetween_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone)
{
	bool CreatedCorners = CreateCornerTransitionBetween(ForCharacterClass, SourceZone, TargetZone);
	bool CreatedJumps = CreateJumpTransitionBetween(ForCharacterClass, SourceZone, TargetZone);

	return CreatedCorners || CreatedJumps;
}

bool UAdvKitBuildModule_Ledge::CreateCornerTransitionBetween(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone)
{
	//Need to know character's movement properties to build transitions
	auto MovementComp = GetCharacterMovementComponent(ForCharacterClass);
	if (!MovementComp)
	{
		return false;
	}

	FVector CharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, SourceZone);
	float CharacterRadius = CharacterHalfExtent.X;
	float CharacterHalfHeight = CharacterHalfExtent.Z;

	FVector OtherCharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, TargetZone);

	//Check how ledges are positioned to each other
	float ForwardDot = FVector::DotProduct(TargetZone->GetActorForwardVector(), SourceZone->GetActorForwardVector());
	FVector LocalClosestOther = SourceZone->GetTransform().InverseTransformPositionNoScale(TargetZone->ConstrainPositionToZone(SourceZone->GetActorLocation(), OtherCharacterHalfExtent));
	FVector LocalClosestSelf = SourceZone->GetTransform().InverseTransformPositionNoScale(SourceZone->ConstrainPositionToZone(TargetZone->GetActorLocation(), CharacterHalfExtent));

	FVector ClosestDistance = LocalClosestOther - LocalClosestSelf;

	FVector TransitionSource = SourceZone->ConstrainPositionToZone(TargetZone->GetActorLocation(), CharacterHalfExtent);
	FVector TransitionTarget = TargetZone->ConstrainPositionToZone(SourceZone->GetActorLocation(), OtherCharacterHalfExtent);

	FVector LocalTransitionSource = SourceZone->GetTransform().InverseTransformPositionNoScale(TransitionSource);

	//Ledge has to be on the same Z plane
	if (FMath::Abs<float>(ClosestDistance.Z) > ErrorMarginPosition)
	{
		return false;
	}
	//Ledge has to be perpendicular (corner)
	if (FMath::Abs<float>(ForwardDot) > ErrorMarginDot)
	{
		return false;
	}

	//Check if corner locations match up with traversable corner
	float PredictedTransitionLengthOuterCorner = FMath::Sqrt(FMath::Pow(CharacterHalfExtent.Y + LocalTransitionSource.X*2, 2.0f)*2.0f);
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

	//Create transition
	auto Transition = CreateTransitionPoint("Corner",SourceZone, TransitionSource, TransitionTarget, ForCharacterClass, TargetZone->GetPhysics(), TargetZone);
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

bool UAdvKitBuildModule_Ledge::CreateJumpTransitionBetween(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone)
{
	//Need to know character's movement properties to build transitions
	auto MovementComp = GetCharacterMovementComponent(ForCharacterClass);
	if (!MovementComp)
	{
		return false;
	}

	FVector CharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, SourceZone);
	FVector OtherCharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, TargetZone);

	float ForwardDot = FVector::DotProduct(TargetZone->GetActorForwardVector(), SourceZone->GetActorForwardVector());
	if (ForwardDot <= (1.0f - ErrorMarginDot))
	{
		//Ledges are not facing in the same direction
		return false;
	}

	FVector LocalClosestOther = SourceZone->GetTransform().InverseTransformPositionNoScale(TargetZone->ConstrainPositionToZone(SourceZone->GetActorLocation(), OtherCharacterHalfExtent));
	FVector LocalClosestSelf = SourceZone->GetTransform().InverseTransformPositionNoScale(SourceZone->ConstrainPositionToZone(TargetZone->GetActorLocation(), CharacterHalfExtent));

	FVector ClosestDistance = LocalClosestOther - LocalClosestSelf;
	//Ledge has to be on the same YZ plane
	if (FMath::Abs<float>(ClosestDistance.X) > ErrorMarginPosition)
	{
		return false;
	}

	FVector TransitionSource = SourceZone->ConstrainPositionToZone(TargetZone->GetActorLocation(), CharacterHalfExtent);
	FVector TransitionTarget = TargetZone->ConstrainPositionToZone(SourceZone->GetActorLocation(), OtherCharacterHalfExtent);

	//Ledge is on the same Z plane
	if (FMath::Abs<float>(ClosestDistance.Z)<ErrorMarginPosition)
	{
		return CreateSideJumpTransitionBetween(ForCharacterClass, SourceZone, TargetZone, TransitionSource, TransitionTarget);
	}

	//Check if the character can jump up or down
	if (FMath::Abs<float>(ClosestDistance.Z) > MovementComp->LedgeJumpDistanceVertical + ErrorMarginPosition)
	{
		return false;
	}

	//If a vertical jump is possible, get overlap of the two zones for an area transition
	auto SourceZoneLine = Cast<AAdvKitZoneLine>(SourceZone);
	auto TargetZoneLine = Cast<AAdvKitZoneLine>(TargetZone);

	FVector OtherMin = TargetZoneLine->ConstrainPositionToZone(SourceZoneLine->GetZoneStartWorld(), OtherCharacterHalfExtent);
	FVector OtherMax = TargetZoneLine->ConstrainPositionToZone(SourceZoneLine->GetZoneEndWorld(), OtherCharacterHalfExtent);

	FVector SelfMin = SourceZoneLine->ConstrainPositionToZone(OtherMin, CharacterHalfExtent);
	FVector SelfMax = SourceZoneLine->ConstrainPositionToZone(OtherMax, CharacterHalfExtent);

	OtherMin = TargetZoneLine->ConstrainPositionToZone(SelfMin, OtherCharacterHalfExtent);
	OtherMax = TargetZoneLine->ConstrainPositionToZone(SelfMax, OtherCharacterHalfExtent);

	//Transition direction have to line up with up vector
	if (FMath::Abs<float>(FVector::DotProduct((OtherMin - SelfMin).GetSafeNormal(), SourceZoneLine->GetActorUpVector())) < (1.0f - ErrorMarginDot)
		|| FMath::Abs<float>(FVector::DotProduct((OtherMax - SelfMax).GetSafeNormal(), SourceZoneLine->GetActorUpVector())) < (1.0f - ErrorMarginDot))
	{
		return false;
	}

	UAdvKitTransitionComponent* NewTransition = nullptr;
	//Create new transition area
	if (SelfMin != SelfMax)
	{
		NewTransition = CreateTransitionArea("VerticalJump", SourceZoneLine,SelfMin, OtherMin, SelfMax, OtherMax, ForCharacterClass, TargetZoneLine->GetPhysics(), TargetZoneLine);
	}
	//Only a single point overlapping
	else if (SelfMin == SelfMax)
	{
		NewTransition = CreateTransitionPoint("VerticalJump",SourceZoneLine, SelfMin, OtherMin, ForCharacterClass, TargetZoneLine->GetPhysics(), TargetZoneLine);
	}

	if (!NewTransition)
	{
		return false;
	}

	NewTransition->CreatedByBuildModule = this;

	//Transition is vertical, needs jump
	if (NewTransition->TransitionDirection.Z > 0)
	{
		NewTransition->bNeedsJump = true;
	}

	return true;
}

bool UAdvKitBuildModule_Ledge::CreateSideJumpTransitionBetween(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone, FVector GlobalSoure, FVector GlobalTarget)
{
	auto SourceLine = Cast<AAdvKitZoneLine>(SourceZone);

	//If transition points are the same, it's not a jump, but an instant change of zone
	if (FMath::IsNearlyZero(FVector::Dist(GlobalSoure, GlobalTarget)))
	{
		auto Transition = CreateTransitionPoint("Instant",SourceZone,GlobalSoure, GlobalTarget, ForCharacterClass, TargetZone->GetPhysics(), TargetZone);
		if (!Transition)
		{
			return false;
		}

		Transition->CreatedByBuildModule = this;

		FVector LocalCenter = (SourceLine->ZoneStart + SourceLine->ZoneEnd) / 2.0f;
		FVector LocalSourceDirection = SourceZone->GetTransform().InverseTransformPosition(GlobalSoure) - LocalCenter;
		Transition->TransitionDirection = LocalSourceDirection.GetSafeNormal();
		return true;
	}

	//Need to know character's movement properties to build transitions
	auto MovementComp = GetCharacterMovementComponent(ForCharacterClass);
	if (!MovementComp)
	{
		return false;
	}

	float JumpDistance = MovementComp->LedgeJumpDistanceHorizontal;

	FVector CharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, SourceZone);
	FVector OtherCharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, TargetZone);

	FVector JumpDirection = (GlobalTarget - GlobalSoure).GetSafeNormal();

	FVector ClosestOther = TargetZone->ConstrainPositionToZone(GlobalSoure + JumpDirection*JumpDistance, OtherCharacterHalfExtent);

	//Check if the character can jump to it right or left
	float LocationDistance = FVector::Dist(GlobalSoure, ClosestOther);
	if (LocationDistance > JumpDistance + ErrorMarginPosition)
	{
		return false;
	}

	//Create transition point
	auto Transition = CreateTransitionPoint("SideJump", SourceZone,GlobalSoure, ClosestOther, ForCharacterClass, TargetZone->GetPhysics(), TargetZone);
	if (!Transition)
	{
		return false;
	}

	Transition->CreatedByBuildModule = this;
	//Mark transition as jump point if desired
	Transition->bNeedsJump = bSidewaysTransitionRequiresJump;
	return true;
}

void UAdvKitBuildModule_Ledge::CreateExitTransitions(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	if (!GEngine)
	{
		return;
	}

	auto LineZone = Cast<AAdvKitZoneLine>(ForZone);
	check(LineZone);

	UWorld* World = GEngine->GetWorldFromContextObject(LineZone);
	if (!World)
	{
		return;
	}

	auto MovementComp = GetCharacterMovementComponent(ForCharacterClass);
	if (!MovementComp)
	{
		return;
	}

	FVector CharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, LineZone);
	float CharacterRadius = CharacterHalfExtent.X;
	float CharacterHalfHeight = CharacterHalfExtent.Z;

	FVector OnLedgeCenter = FMath::Lerp<FVector>(LineZone->GetZoneStartWorld(), LineZone->GetZoneEndWorld(), 0.5f);

	//Creates a transition to the floor beneath
	if (bCreateTransitionToFloor)
	{
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(LineZone);
		FCollisionObjectQueryParams ObjectQueryParams;

		float LocalFloorTraceDistance = FloorTraceDistance;
		FHitResult Hit;
		if (World->LineTraceSingleByObjectType(Hit, OnLedgeCenter, OnLedgeCenter - LineZone->GetActorUpVector() * LocalFloorTraceDistance, ObjectQueryParams, QueryParams))
		{
			LocalFloorTraceDistance = FVector::Dist(OnLedgeCenter, Hit.ImpactPoint);
		}

		FVector Floor = OnLedgeCenter - LineZone->GetActorUpVector() * LocalFloorTraceDistance;
		FVector CharacterOffsetFromGround = LineZone->GetTransform().TransformVector(FVector(CharacterRadius, 0, CharacterHalfHeight));
		FVector GroundZoneExtent(10, FMath::Abs<float>(LineZone->ZoneStart.Y - LineZone->ZoneEnd.Y) / 2.0f - CharacterRadius, 10);
		FVector YExtent = LineZone->GetActorRightVector()*GroundZoneExtent.Y;

		FVector FloorZonePosition = Floor + CharacterOffsetFromGround;// +LineZone->GetActorUpVector() * GroundZoneExtent.Z;// +Zone->GetActorForwardVector() * GroundZoneExtent.X;
		auto NewTransition = CreateTransitionArea("Floor",LineZone,OnLedgeCenter - YExtent, FloorZonePosition - YExtent, OnLedgeCenter + YExtent, FloorZonePosition + YExtent, ForCharacterClass, EAdvKitMovementMode::Falling, NULL);
		if (NewTransition)
		{
			NewTransition->CreatedByBuildModule = this;
			NewTransition->TransitionDirection = -FVector::UpVector;
		}
	}

	//Creates a transition to the platform above
	if (!bCreateTransitionToPlatform)
	{
		return;
	}

	FVector GroundZoneExtent(10, FMath::Abs<float>(LineZone->ZoneStart.Y - LineZone->ZoneEnd.Y) / 2.0f - CharacterRadius, 10);
	FVector Floor = OnLedgeCenter - LineZone->GetActorForwardVector() * (GroundZoneExtent.X * 2);
	FVector YExtent = LineZone->GetActorRightVector()*GroundZoneExtent.Y;

	FQuat ZoneQuat = LineZone->GetActorQuat() * FRotator(0, 180, 0).Quaternion();
	FVector FloorZonePosition = Floor + LineZone->GetActorUpVector() * CharacterHalfHeight - LineZone->GetActorForwardVector() * CharacterRadius;
	auto Transition = CreateTransitionArea("Platform",LineZone,OnLedgeCenter - YExtent, FloorZonePosition - YExtent, OnLedgeCenter + YExtent, FloorZonePosition + YExtent, ForCharacterClass, EAdvKitMovementMode::Walking, NULL/*GroundZone*/);
	if (!Transition)
	{
		return;
	}

	Transition->CreatedByBuildModule = this;
	Transition->TransitionDirection = FVector::UpVector;
	Transition->bNeedsJump = true;
}
