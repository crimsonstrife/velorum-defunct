// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/Movement/AdvKitCA_ZoneLocationSwitch.h"
#include "Actions/Movement/AdvKitCA_ZoneTransition_Arguments.h"

#include "Environment/Zones/AdvKitZoneLine.h"
#include "Environment/AdvKitZoneLocation.h"

#include "Player/AdvKitCharacter.h"


FAdvKitActionResponse UAdvKitCA_ZoneLocationSwitch::Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments, UAdvKitCharacterAction* InterruptedOther)
{
	auto SuperResult = Super::Start_Implementation(Arguments, InterruptedOther);

	if (SuperResult.IsFailure())
	{
		return SuperResult;
	}

	//Only valid with transition arguments
	auto ZoneArgs = Cast<UAdvKitCA_ZoneTransition_Arguments >(Arguments);
	if (!IsValid(ZoneArgs))
	{
		return Failed();
	}

	//Only valid if zone is a line zone
	auto* Zone = Cast<AAdvKitZoneLine>(ZoneArgs->Zone ? ZoneArgs->Zone : CharacterOwner->GetZone());
	if (!Zone)
	{
		return Failed();
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
		return Redirect(ActionAtStartPoint, Arguments);
	}
	
	return Redirect(ActionAtEndPoint, Arguments);
}

TArray<TSubclassOf<UAdvKitCharacterAction>> UAdvKitCA_ZoneLocationSwitch::GetAdditionalRequiredActions_Implementation()
{
	TArray<TSubclassOf<UAdvKitCharacterAction>> AdditionalActions;

	AdditionalActions.AddUnique(ActionAtStartPoint);
	AdditionalActions.AddUnique(ActionAtEndPoint);

	return AdditionalActions;
}

