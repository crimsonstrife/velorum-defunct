// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Environment/AdvKitZone.h"
#include "Environment/AdvKitZoneLocation.h"
#include "Environment/Transitions/AdvKitTransitionComponentPoint.h"
#include "Environment/Transitions/AdvKitTransitionComponentArea.h"
#include "Environment/AdvKitTransitionBuilderModule.h"
#include "Player/AdvKitCharacter.h"

AAdvKitZone::AAdvKitZone(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ZonePhysics = EAdvKitMovementMode::Falling;
	CustomZonePhysics = 0;
	bDoubleConstructionScriptFix = false;
}

void AAdvKitZone::GenerateTransitions_Implementation(bool bZoneMoved)
{
	if (!bDoubleConstructionScriptFix)
	{
		bDoubleConstructionScriptFix = true;
		return;
	}

	//Zones that were targeted by transition generation
	TArray<AAdvKitZone*> AffectedZones;

	//Let all modules build transitions
	for (auto Module : BuildModules)
	{
		//Skip if module is none
		if (!Module)
		{
			UE_LOG(LogAdvKit, Error, TEXT("Invalid build module on zone %s!"), *GetName())
			continue;
		}

		//Create transitions
		TArray<AAdvKitZone*> NewAffectedZones;
		Module->CreateTransitions(NewAffectedZones);

		for (auto NewZone : NewAffectedZones)
		{
			AffectedZones.AddUnique(NewZone);
		}
	}

	//If the zone moved and affected transitions the corresponding ones need to be rebuild too
	if (!bZoneMoved)
	{
		//Zone did not move
		return;
	}

	//Regenerate transitions on affected zones
	for (auto AffectedZone : AffectedZones)
	{
		if (!AffectedZone)
		{
			continue;
		}

		//TODO provide instigator zone, so generate transitions does not regenerate all transitions
		AffectedZone->GenerateTransitions(false);
	}
}

UAdvKitTransitionComponentPoint* AAdvKitZone::RegisterTransition(const FVector& WorldSourceLocation, const FVector& WorldTargetLocation, TSubclassOf<AAdvKitCharacter> ForCharacter, EAdvKitMovementMode MovementMode, class AAdvKitZone* TargetZone, uint8 CustomMovementMode)
{
	//Create new transition object
	FName NewName = MakeUniqueObjectName(GetOuter(), UAdvKitTransitionComponentPoint::StaticClass(), FName("TransitionPoint"));
	auto Template = UAdvKitTransitionComponentPoint::StaticClass()->GetDefaultObject();
	auto NewComponent = CreateComponentFromTemplate(Cast<UAdvKitTransitionComponentPoint>(Template), NewName);
	auto NewTransition = Cast<UAdvKitTransitionComponentPoint>(NewComponent);

	//New transition was not created
	if (!NewTransition)
	{
		return nullptr;
	}

	//Add component
	BlueprintCreatedComponents.Add(NewTransition);
	NewTransition->CreationMethod = EComponentCreationMethod::Instance; //TODO change to instance and maybe manage updating already existing transitions
	NewTransition->RegisterComponent();
	
	//Initialize transition
	NewTransition->Init(ForCharacter,WorldSourceLocation, WorldTargetLocation, MovementMode, TargetZone, CustomMovementMode);
	return NewTransition;
}

UAdvKitTransitionComponentArea* AAdvKitZone::RegisterTransitionArea(const FVector& MinWorldSourceLocation, const FVector& MinWorldTargetLocation, const FVector& MaxWorldSourceLocation, const FVector& MaxWorldTargetLocation, TSubclassOf<AAdvKitCharacter> ForCharacter, EAdvKitMovementMode MovementMode, class AAdvKitZone* TargetZone, uint8 CustomMovementMode)
{
	//Create new transition object
	FName NewName = MakeUniqueObjectName(GetOuter(), UAdvKitTransitionComponentArea::StaticClass(), FName("TransitionArea"));
	auto Template = UAdvKitTransitionComponentArea::StaticClass()->GetDefaultObject();
	auto NewComponent = CreateComponentFromTemplate(Cast<UAdvKitTransitionComponentArea>(Template), NewName);
	auto NewTransition = Cast<UAdvKitTransitionComponentArea>(NewComponent);

	//New transition was not created
	if (!NewTransition)
	{
		return nullptr;
	}

	//Add component
	BlueprintCreatedComponents.Add(NewTransition);
	NewTransition->CreationMethod = EComponentCreationMethod::Instance; //TODO change to instance and maybe manage updating already existing transitions
	NewTransition->RegisterComponent();

	//Initialize transition
	NewTransition->Init(ForCharacter, MinWorldSourceLocation, MinWorldTargetLocation, MaxWorldSourceLocation, MaxWorldTargetLocation, MovementMode, TargetZone, CustomMovementMode);
	return NewTransition;
}

void AAdvKitZone::AddTransition(TSubclassOf<AAdvKitCharacter> For, class UAdvKitTransitionComponent* Transition)
{
	check(false);

	////Try to add transition to an existing configuration
	//for (FAdvKitZoneTransitions& TransitionList : Transitions)
	//{
	//	if (TransitionList.UseableBy->IsChildOf(For))
	//	{
	//		TransitionList.Targets.AddUnique(Transition);
	//		return;
	//	}
	//}

	////No existing configuration for the character exists, so add a new one
	//FAdvKitZoneTransitions NewTransitionList;
	//NewTransitionList.Targets.Add(Transition);
	//NewTransitionList.UseableBy = For;

	//Transitions.Add(NewTransitionList);

}

void AAdvKitZone::RemoveExistingTransitionsTo(class AAdvKitZone* Other)
{
	auto Transitions = GetComponentsByClass(UAdvKitTransitionComponent::StaticClass());

	//Go through all configurations
	for (auto Component : Transitions)
	{
		auto TransitionComponent = Cast<UAdvKitTransitionComponent>(Component);
		//Test all transitions
		if (TransitionComponent->TargetZone == Other)
		{
			TransitionComponent->DestroyComponent();
		}
	}
}

bool AAdvKitZone::IsPointInside(const FVector& WorldPosition, float ErrorMargin)
{
	return FVector::Dist(ConstrainPositionToZone(WorldPosition, FVector::ZeroVector), WorldPosition) < ErrorMargin;
}

TArray<class UAdvKitTransitionComponent*> AAdvKitZone::GetPossibleTransitions(const class AAdvKitCharacter* ForCharacter)
{
	TArray<class UAdvKitTransitionComponent*> OutArray;

	auto Transitions = GetComponentsByClass(UAdvKitTransitionComponent::StaticClass());

	//Go through all transitions
	for (auto Component : Transitions)
	{
		auto TransitionComponent = Cast<UAdvKitTransitionComponent>(Component);
		
		//Check if usable by character
		if (IsValid(TransitionComponent->UseableBy) && !ForCharacter->IsA(TransitionComponent->UseableBy))
		{
			continue;
		}

		//Add all transitions to the result
		OutArray.Add(TransitionComponent);
	}

	return OutArray;
}

bool AAdvKitZone::MoveLocationInDirection(UAdvKitZoneLocation* LocationToMove, const FVector& WorldDesiredDelta, const FVector& HalfExtent, const FRotator& ExtentWorldRotation)
{
	static const float ZeroTolerance = 0.001f;
	check(LocationToMove);

	FVector OldLocation = LocationToMove->GetGlobalPosition();

	FVector NewLocation = GetPositionInDirection(OldLocation, WorldDesiredDelta, true, HalfExtent, ExtentWorldRotation);
	LocationToMove->Set(this, NewLocation);

	//Check if location has moved
	return !(NewLocation - OldLocation).IsNearlyZero(ZeroTolerance);
}

FVector AAdvKitZone::GetPositionInDirection_Implementation(const FVector& FromWorldPosition, const FVector& WorldDesiredDelta, bool bClamp, const FVector& HalfExtent, const FRotator& ExtentWorldRotation)
{
	FVector AdjustedDirection = ConstrainDirectionToZone(WorldDesiredDelta, FromWorldPosition);
	if (bClamp)
	{
		return ConstrainPositionToZone(FromWorldPosition + AdjustedDirection, HalfExtent, ExtentWorldRotation);
	}

	return ConstrainPositionToZone(FromWorldPosition, HalfExtent, ExtentWorldRotation) + AdjustedDirection;
}

FVector AAdvKitZone::GetZoneForwardVector(const UAdvKitZoneLocation* AtLocation)
{
	return GetActorForwardVector();
}

FVector AAdvKitZone::GetZoneRightVector(const UAdvKitZoneLocation* AtLocation)
{
	return GetActorRightVector();
}

FVector AAdvKitZone::GetZoneUpVector(const UAdvKitZoneLocation* AtLocation)
{
	return GetActorUpVector();
}


FVector AAdvKitZone::ConstrainPositionToZone_Implementation(const FVector& WorldPosition, const FVector& HalfExtent, const FRotator& ExtentWorldRotation)
{
	return GetActorLocation();
}

UAdvKitZoneLocation* AAdvKitZone::GetClosestZoneLocation_Implementation(const FVector& WorldPosition, const FVector& HalfExtent, const FRotator& ExtentWorldRotation)
{
	return nullptr;
}

FVector AAdvKitZone::ConstrainDirectionToZone_Implementation(const FVector& WorldDirection, const FVector& AtWorldLocation)
{
	return WorldDirection;
}

EAdvKitMovementMode AAdvKitZone::GetPhysics()
{
	return ZonePhysics;
}

uint8 AAdvKitZone::GetCustomPhysics()
{
	return CustomZonePhysics;
}

bool AAdvKitZone::HasPhysics(EAdvKitMovementMode QueryPhysics)
{
	return GetPhysics() == QueryPhysics;
}

bool AAdvKitZone::HasCustomPhysics(uint8 QueryPhysics)
{
	return GetCustomPhysics() == QueryPhysics;
}

void AAdvKitZone::SetCharacterClass(TSubclassOf<AAdvKitCharacter> CharacterClass)
{
	for (auto Module : BuildModules)
	{
		if (!Module)
		{
			continue;
		}

		Module->DefaultCharacterClasses.Empty();
		Module->DefaultCharacterClasses.AddUnique(CharacterClass);
	}
}

void AAdvKitZone::AddCharacterClass(TSubclassOf<AAdvKitCharacter> CharacterClass)
{
	for (auto Module : BuildModules)
	{
		if (!Module)
		{
			continue;
		}

		Module->DefaultCharacterClasses.AddUnique(CharacterClass);
	}
}

void AAdvKitZone::RemoveCharacterClass(TSubclassOf<AAdvKitCharacter> CharacterClass)
{
	for (auto Module : BuildModules)
	{
		if (!Module)
		{
			continue;
		}

		Module->DefaultCharacterClasses.Remove(CharacterClass);
	}
}

bool AAdvKitZone::IsCharacterSupported(AAdvKitCharacter* Character)
{
	if (!Character)
	{
		return false;
	}

	if(BuildModules.Num()==0)
	{
		return true;
	}
	
	for (auto Module : BuildModules)
	{
		if (!Module)
		{
			continue;
		}

		for (auto CharacterClass : Module->DefaultCharacterClasses)
		{
			if (Character->IsA(CharacterClass))
			{
				return true;
			}
		}
	}

	return false;
}
