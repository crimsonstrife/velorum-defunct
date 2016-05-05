// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Environment/Zones/AdvKitZoneLine.h"

AAdvKitZoneLine::AAdvKitZoneLine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ZonePhysics = EAdvKitMovementMode::Falling;

	ZoneStart = FVector(-100, 0, 0);
	ZoneEnd = FVector(100, 0, 0);

	bConstrainExtentAtStart = true;
	bConstrainExtentAtEnd = true;
}

FVector AAdvKitZoneLine::GetZoneStartWorld()
{
	return GetTransform().TransformPositionNoScale(ZoneStart);
}

FVector AAdvKitZoneLine::GetZoneEndWorld()
{
	return GetTransform().TransformPositionNoScale(ZoneEnd);
}

FVector AAdvKitZoneLine::ConstrainPositionToZone_Implementation(const FVector& WorldPosition, const FVector& HalfExtent, const FRotator& ExtentWorldRotation)
{
	//Convert position to local space
	FVector PositionLocal = GetTransform().InverseTransformPositionNoScale(WorldPosition);

	//Get local direction
	FVector ZoneDirection = (ZoneEnd - ZoneStart).GetSafeNormal();

	//Get local projected extent
	FVector ZoneDirectionOffset = GetProjectedHalfExtent(ZoneDirection, HalfExtent, ExtentWorldRotation);

	//Set start/end offsets
	FVector StartOffset = bConstrainExtentAtStart ? ZoneDirectionOffset : FVector::ZeroVector;
	FVector EndOffset = bConstrainExtentAtEnd ? ZoneDirectionOffset : FVector::ZeroVector;

	//Constrain local location to line
	FVector ClosestLocalPosition = FMath::ClosestPointOnLine(ZoneStart + StartOffset, ZoneEnd - EndOffset, PositionLocal);

	//DrawDebugLine(GetWorld(), GetZoneStartWorld(), GetZoneEndWorld(), FColor::Yellow);
	//DrawDebugCoordinateSystem(GetWorld(), GetTransform().TransformPositionNoScale(ClosestLocalPosition), GetActorRotation(), 150);

	//Transform result to world space
	return GetTransform().TransformPositionNoScale(ClosestLocalPosition);
}

FVector AAdvKitZoneLine::ConstrainDirectionToZone_Implementation(const FVector& WorldDirection, const FVector& AtWorldLocation)
{
	//Transform world direction to local space
	FVector DirectionLocal = GetTransform().InverseTransformVectorNoScale(WorldDirection);

	//Get local direction
	FVector ZoneDirection = (ZoneEnd - ZoneStart).GetSafeNormal();

	//Project desired direction on zone direction
	FVector ConstrainedLocalDirection = ZoneDirection * FVector::DotProduct(ZoneDirection, DirectionLocal);

	//Transform result to world space
	return GetTransform().TransformVectorNoScale(ConstrainedLocalDirection);
}


class UAdvKitZoneLocation* AAdvKitZoneLine::GetClosestZoneLocation_Implementation(const FVector& WorldPosition, const FVector& HalfExtent, const FRotator& ExtentWorldRotation)
{
	//Create a location object
	UAdvKitZoneLocation* NewLocation = NewObject<UAdvKitZoneLocation>(this);

	//Initialize it
	NewLocation->Set(this, ConstrainPositionToZone(WorldPosition, HalfExtent, ExtentWorldRotation));
	return NewLocation;
}

FVector AAdvKitZoneLine::GetProjectedHalfExtent(const FVector &LocalDirection, const FVector& HalfExtent, const FRotator& ExtentWorldRotation)
{
	//Transform line direction to local space of extent cylinder
	FVector WorldDirection = GetActorRotation().RotateVector(LocalDirection);
	FVector DirectionInExtentSpace = ExtentWorldRotation.UnrotateVector(WorldDirection);
	DirectionInExtentSpace.Normalize();

	FVector ExtentOffset = FVector::ZeroVector;

	float AbsZ = FMath::Abs<float>(DirectionInExtentSpace.Z);
	//If orientation aligns with up vector only the height matters
	if (FMath::IsNearlyEqual(AbsZ, 1.0f))
	{
		ExtentOffset = FVector::UpVector * HalfExtent;
	}
	//If orientation aligns with line only radius matters
	else if (FMath::IsNearlyZero(AbsZ))
	{
		ExtentOffset = DirectionInExtentSpace * HalfExtent;
	}
	//Orientation does not simply match horizontal or vertical, more math is needed
	else
	{
		//Find intersection distance with radius
		FVector FlatVector = FVector(DirectionInExtentSpace.X, DirectionInExtentSpace.Y, 0);
		float radiusDistance = FMath::Abs<float>(HalfExtent.X / FlatVector.Size());
		
		//Find intersection distance with cylinder top
		float upDistance = FMath::Abs<float>(HalfExtent.Z / DirectionInExtentSpace.Z);

		//Use closest distance and multiply with direction
		ExtentOffset = DirectionInExtentSpace * FMath::Min<float>(radiusDistance, upDistance);
	}

	WorldDirection = ExtentWorldRotation.RotateVector(ExtentOffset);
	return GetActorRotation().UnrotateVector(WorldDirection);
}