// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"

#include "AdvKitTypes.h"
#include "Player/AdvKitCharacter.h"
#include "Environment/Zones/AdvKitZoneLine.h"
#include "Environment/BuildModules/AdvKitBuildModule_Ledge_Tightspace.h"
#include "Environment/Transitions/AdvKitTransitionComponentPoint.h"
#include "Environment/Transitions/AdvKitTransitionComponentArea.h"

bool UAdvKitBuildModule_Ledge_Tightspace::CanCreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	//Cannot create transitions on non line zone object
	if (!ForZone->IsA(AAdvKitZoneLine::StaticClass()))
	{
		return false;
	}

	//Only valid for these movement modes
	if (ForZone->GetPhysics() != EAdvKitMovementMode::ClimbingLedge
		&& ForZone->GetPhysics() != EAdvKitMovementMode::WalkingTightspace)
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
	return MovementProps->bCanClimbLedge && MovementProps->bCanWalkTightSpace;
}


bool UAdvKitBuildModule_Ledge_Tightspace::GatherPotentialTargetZones_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone, TArray<AAdvKitZone*>& OutZones)
{
	OutZones.Empty();

	auto LineZone = Cast<AAdvKitZoneLine>(ForZone);
	check(LineZone);

	//Check for zones that almost or actually touch one another
	FVector CharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, LineZone);
	TArray<AAdvKitZone*> TempZones;
	if (!OverlapForCloseZones(LineZone, LineZone->GetZoneStartWorld(), LineZone->GetZoneEndWorld(), CharacterHalfExtent.Z * 2, TempZones))
	{
		return false;
	}

	//Filter targets
	for (auto Zone : TempZones)
	{
		//Only works for lines
		if (!Zone->IsA(AAdvKitZoneLine::StaticClass()))
		{
			continue;
		}

		//Cannot create transitions from tight space to anything other than ledge
		if (LineZone->GetPhysics() == EAdvKitMovementMode::WalkingTightspace
			&& Zone->GetPhysics() == EAdvKitMovementMode::ClimbingLedge)
		{
			OutZones.Add(Zone);
			continue;
		}

		//Cannot create transitions from ledge to anything other than tight space
		if (LineZone->GetPhysics() == EAdvKitMovementMode::ClimbingLedge
			&& Zone->GetPhysics() == EAdvKitMovementMode::WalkingTightspace)
		{
			OutZones.Add(Zone);
		}
	}

	return OutZones.Num() > 0;
}

bool UAdvKitBuildModule_Ledge_Tightspace::CreateTransitionBetween_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone)
{
	if (SourceZone->GetPhysics() == EAdvKitMovementMode::ClimbingLedge)
	{
		return CreateTransitionLedgeToTightSpace(ForCharacterClass, SourceZone, TargetZone);
	}

	if (SourceZone->GetPhysics() == EAdvKitMovementMode::WalkingTightspace)
	{
		return CreateTransitionTightSpaceToLedge(ForCharacterClass, SourceZone, TargetZone);
	}

	return false;
}

bool UAdvKitBuildModule_Ledge_Tightspace::CreateTransitionLedgeToTightSpace(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* Ledge, AAdvKitZone* TightSpace)
{
	auto LedgeLine = Cast<AAdvKitZoneLine>(Ledge);
	auto TightSpaceLine = Cast<AAdvKitZoneLine>(TightSpace);

	FVector CharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, LedgeLine);
	FVector OtherCharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, TightSpaceLine);

	//Zones have to be parallel
	float Dot = FVector::DotProduct(TightSpaceLine->GetActorForwardVector(), LedgeLine->GetActorForwardVector());
	if (Dot < (1.0f - ErrorMarginDot))
	{
		return false;
	}

	//Project zones onto each other
	FVector OtherMin = TightSpaceLine->ConstrainPositionToZone(LedgeLine->GetZoneStartWorld(), OtherCharacterHalfExtent);
	FVector OtherMax = TightSpaceLine->ConstrainPositionToZone(LedgeLine->GetZoneEndWorld(), OtherCharacterHalfExtent);

	FVector SelfMin = LedgeLine->ConstrainPositionToZone(OtherMin, CharacterHalfExtent);
	FVector SelfMax = LedgeLine->ConstrainPositionToZone(OtherMax, CharacterHalfExtent);

	OtherMin = TightSpaceLine->ConstrainPositionToZone(SelfMin, OtherCharacterHalfExtent);
	OtherMax = TightSpaceLine->ConstrainPositionToZone(SelfMax, OtherCharacterHalfExtent);

	UAdvKitTransitionComponent* Transition = nullptr;
	//Area overlapping
	if (SelfMin != SelfMax)
	{
		Transition = CreateTransitionArea("ToTightSpace", LedgeLine, SelfMin, OtherMin, SelfMax, OtherMax, ForCharacterClass, TightSpaceLine->GetPhysics(), TightSpaceLine);
	}
	//Only a single point overlapping
	else if (SelfMin == SelfMax)
	{
		Transition = CreateTransitionPoint("ToTightSpace", LedgeLine, SelfMin, OtherMin, ForCharacterClass, TightSpaceLine->GetPhysics(), TightSpaceLine);
	}

	if (!Transition)
	{
		return false;
	}

	//Transition from ledge to tight space should be up vertically
	Transition->TransitionDirection = FVector::UpVector;
	Transition->bNeedsJump = true;
	return true;
}

bool UAdvKitBuildModule_Ledge_Tightspace::CreateTransitionTightSpaceToLedge(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* TightSpace, AAdvKitZone* Ledge)
{
	auto LedgeLine = Cast<AAdvKitZoneLine>(Ledge);
	auto TightSpaceLine = Cast<AAdvKitZoneLine>(TightSpace);

	FVector CharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, TightSpaceLine);
	FVector OtherCharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, LedgeLine);

	//Zones have to be parallel
	float Dot = FVector::DotProduct(TightSpaceLine->GetActorForwardVector(), LedgeLine->GetActorForwardVector());
	if (Dot < (1.0f - ErrorMarginDot))
	{
		return false;
	}

	//Check if ledge is below tight space
	FVector ClosestLocationOther = LedgeLine->ConstrainPositionToZone(TightSpaceLine->GetActorLocation(), OtherCharacterHalfExtent);
	FVector ClosestLocationSelf = TightSpaceLine->ConstrainPositionToZone(ClosestLocationOther, CharacterHalfExtent);
	FVector LocalLocationOther = TightSpaceLine->GetTransform().InverseTransformPositionNoScale(ClosestLocationOther);
	FVector LocalLocationSelf = TightSpaceLine->GetTransform().InverseTransformPositionNoScale(ClosestLocationSelf);
	if (LocalLocationOther.Z >= LocalLocationSelf.Z)
	{
		return false;
	}

	//Project zones onto each other
	FVector OtherMin = LedgeLine->ConstrainPositionToZone(TightSpaceLine->GetZoneStartWorld(), OtherCharacterHalfExtent);
	FVector OtherMax = LedgeLine->ConstrainPositionToZone(TightSpaceLine->GetZoneEndWorld(), OtherCharacterHalfExtent);

	FVector SelfMin = TightSpaceLine->ConstrainPositionToZone(OtherMin, CharacterHalfExtent);
	FVector SelfMax = TightSpaceLine->ConstrainPositionToZone(OtherMax, CharacterHalfExtent);

	OtherMin = LedgeLine->ConstrainPositionToZone(SelfMin, OtherCharacterHalfExtent);
	OtherMax = LedgeLine->ConstrainPositionToZone(SelfMax, OtherCharacterHalfExtent);

	UAdvKitTransitionComponent* Transition = nullptr;
	//Area overlapping
	if (SelfMin != SelfMax)
	{
		Transition = CreateTransitionArea("ToLedge",TightSpaceLine, SelfMin, OtherMin, SelfMax, OtherMax, ForCharacterClass, LedgeLine->GetPhysics(), LedgeLine);
	}
	//Only a single point overlapping
	else if (SelfMin == SelfMax)
	{
		Transition = CreateTransitionPoint("ToLedge", TightSpaceLine, SelfMin, OtherMin, ForCharacterClass, LedgeLine->GetPhysics(), LedgeLine);
	}

	if (!Transition)
	{
		return false;
	}

	//Transition from tight space to ledge should be down vertically
	Transition->TransitionDirection = -FVector::UpVector;
	return true;
}
