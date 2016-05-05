// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Environment/AdvKitZoneLocation.h"
#include "Environment/AdvKitZone.h"

void UAdvKitZoneLocation::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAdvKitZoneLocation, Zone);
	DOREPLIFETIME(UAdvKitZoneLocation, LocalPosition);
}

void UAdvKitZoneLocation::Set(class AAdvKitZone* InZone, const FVector& InPosition)
{
	LocalPosition = InPosition;
	Zone = InZone;

	if (Zone)
	{
		LocalPosition = Zone->GetTransform().InverseTransformPositionNoScale(InPosition);
	}
}

FVector UAdvKitZoneLocation::GetGlobalPosition() const
{
	if (!Zone)
	{
		return LocalPosition;
	}


	return Zone->GetTransform().TransformPositionNoScale(LocalPosition); 
}

FVector UAdvKitZoneLocation::ConstrainDirection(const FVector& WorldDirection)
{
	if (!Zone)
	{
		return WorldDirection;
	}

	return Zone->ConstrainDirectionToZone(WorldDirection, GetGlobalPosition());

}

bool UAdvKitZoneLocation::MoveInDirection(const FVector& WorldDesiredDelta, const FVector& HalfExtent, const FRotator& ExtentWorldRotation)
{
	if (!Zone)
	{
		return false;
	}

	return Zone->MoveLocationInDirection(this, WorldDesiredDelta, HalfExtent, ExtentWorldRotation);
}
