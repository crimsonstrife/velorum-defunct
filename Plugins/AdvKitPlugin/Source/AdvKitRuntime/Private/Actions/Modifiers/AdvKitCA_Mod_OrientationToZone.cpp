// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/Modifiers/AdvKitCA_Mod_OrientationToZone.h"
#include "Actions/Movement/AdvKitCA_ZoneTransition.h"
#include "Actions/Movement/AdvKitCA_ZoneTransition_Arguments.h"
#include "Environment/AdvKitZone.h"
#include "Environment/AdvKitTransitionComponent.h"

UAdvKitCA_Mod_OrientationToZone::UAdvKitCA_Mod_OrientationToZone(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bSetLocation = false;
	Location = FVector::ZeroVector;
	bSetRotation = true;
	Rotation = FRotator::ZeroRotator;
}

void UAdvKitCA_Mod_OrientationToZone::ApplyToCharacter(class AAdvKitCharacter* ToCharacter, class UAdvKitCharacterMovementComponent* ToMovementComponent)
{
	auto OwnerAction = GetTypedOuter<UAdvKitCA_ZoneTransition>();
	if(!OwnerAction)
	{
		OwnerAction =  Cast<UAdvKitCA_ZoneTransition>(ToCharacter->GetActiveAction());
	}

	if (!OwnerAction)
	{
		return;
	}

	auto Arguments = OwnerAction->GetCurrentArguments();
	if (!Arguments)
	{
		return;
	}

	auto Zone = Arguments->Zone;
	if (!Zone)
	{
		return;
	}

	if (bSetRotation)
	{
		ToCharacter->SetActorRotation(Zone->GetActorQuat()*Rotation.Quaternion());
	}

	if (bSetLocation)
	{
		ToCharacter->SetActorLocation(Zone->GetTransform().TransformPosition(Location));
	}
}
