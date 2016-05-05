// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/Movement/AdvKitCA_ZoneTransition_LocationSwitch.h"
#include "Actions/Movement/AdvKitCA_ZoneTransition_Arguments.h"
#include "Environment/AdvKitZone.h"
#include "Player/AdvKitCharacter.h"

UAnimMontage* UAdvKitCA_ZoneTransition_LocationSwitch::GetMontageToPlay_Implementation()
{
	//Only valid with transition arguments
	auto Args = GetCurrentArguments();
	if (!Args)
	{
		return nullptr;
	}

	//Only valid if zone is a line zone
	auto Zone = Cast<AAdvKitZoneLine>(Args->Zone ? Args->Zone : CharacterOwner->GetZone());
	if (!Zone)
	{
		return nullptr;
	}

	//Get character attributes
	FVector LocalExtent = MovementComponent->GetHalfExtentForZone(Zone);
	FVector CharacterLocation = CharacterOwner->GetActorLocation();
	FRotator CharacterRotation = CharacterOwner->GetActorRotation();

	//Try to get location in zone
	FVector ClosestLocation = Zone->ConstrainPositionToZone(CharacterLocation, LocalExtent, CharacterRotation);
	bool bCloserToStart = FVector::Dist(ClosestLocation, Zone->GetZoneStartWorld()) < FVector::Dist(ClosestLocation, Zone->GetZoneEndWorld());

	if (bCloserToStart)
	{
		return MontageAtStartPoint;
	}

	return MontageAtEndPoint;
}
