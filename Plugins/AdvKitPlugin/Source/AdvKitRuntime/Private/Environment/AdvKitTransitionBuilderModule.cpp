// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"

#include "Player/AdvKitCharacter.h"
#include "AdvKitTypes.h"

#include "Environment/AdvKitZone.h"
#include "Environment/AdvKitTransitionBuilderModule.h"

UAdvKitTransitionBuilderModule::UAdvKitTransitionBuilderModule(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultCharacterClasses.Add(AAdvKitCharacter::StaticClass());
	ErrorMarginPosition = 3.0f;
	ErrorMarginDot = 0.01f;
}

void UAdvKitTransitionBuilderModule::CreateTransitions_Implementation(TArray<AAdvKitZone*>& OutAffectedZones)
{
	CreatedOrUpdatedTransitions.Empty();
	AffectedZones.Empty();

	auto Zone = GetOuterZone();
	if (!Zone)
	{
		UE_LOG(LogAdvKit, Error, TEXT("Cannot create transition without a zone!"))
		return;
	}

	TArray<UAdvKitTransitionComponent*> AllTransitionsByThisModule = GetPreExistingTransitions(Zone);

	for(auto CharacterClass : DefaultCharacterClasses)
	{ 
		if (!CharacterClass)
		{
			UE_LOG(LogAdvKit, Error, TEXT("Invalid character class!"))
			continue;
		}

		if (!CanCreateTransitionsFor(CharacterClass, Zone))
		{
			UE_LOG(LogAdvKit, Error, TEXT("%s cannot create transitions for zone %s with character %s!"), *GetName(), *Zone->GetName(), *CharacterClass->GetName());
			continue;
		}

		CreateTransitionsFor(CharacterClass, Zone);
	}

	for (auto NewTransition : CreatedOrUpdatedTransitions)
	{
		//UE_LOG(LogAdvKit, Log, TEXT("%s-%s Updated: %s"), *this->GetOuter()->GetName(), *this->GetName(), *NewTransition->TransitionId);
		AllTransitionsByThisModule.Remove(NewTransition);
	}

	for (auto OldTransition : AllTransitionsByThisModule)
	{
		OutAffectedZones.AddUnique(OldTransition->TargetZone.Get());
		//UE_LOG(LogAdvKit, Error, TEXT("%s-%s Deleting: %s"), *this->GetOuter()->GetName(), *this->GetName(),*OldTransition->TransitionId);
		OldTransition->DestroyComponent();
	}

	OutAffectedZones.Append(AffectedZones);
	//for (auto AffectedZone : AffectedZones)
	//{
	//	OutAffectedZones.AddUnique(AffectedZone);
	//}

	CreatedOrUpdatedTransitions.Empty();
	AffectedZones.Empty();
}

AAdvKitZone* UAdvKitTransitionBuilderModule::GetOuterZone()
{
	return GetTypedOuter<AAdvKitZone>();
}

bool UAdvKitTransitionBuilderModule::CanCreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	//Dummy implementation, override in subclass.
	return false;
}

void UAdvKitTransitionBuilderModule::CreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	//Gather potential targets
	TArray<AAdvKitZone*> TargetZones;
	if (!GatherPotentialTargetZones_Implementation(ForCharacterClass, ForZone, TargetZones))
	{
		//No targets means there can be no transitions
		return;
	}

	//Try to create transitions for all potential targets
	for (auto TargetZone : TargetZones)
	{
		if (CreateTransitionBetween_Implementation(ForCharacterClass, ForZone, TargetZone))
		{
			//Created transitions to this target, so the zone was affected
			AffectedZones.AddUnique(TargetZone);
		}
	}
}

bool UAdvKitTransitionBuilderModule::GatherPotentialTargetZones_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone, TArray<AAdvKitZone*>& OutZones)
{
	//Dummy implementation, override in subclass.
	OutZones.Empty();
	return false;
}

bool UAdvKitTransitionBuilderModule::CreateTransitionBetween_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone)
{
	//Dummy implementation, override in subclass.
	return false;
}

const FVector UAdvKitTransitionBuilderModule::GetCharacterHalfExtent(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone) const
{
	check(ForCharacterClass);
	check(ForZone);

	auto DefaultCharacter = GetDefaultCharacterObject(ForCharacterClass);
	check(DefaultCharacter);

	auto DummyMoveComp = DefaultCharacter->GetAdvMovementComponent();
	check(DummyMoveComp);

	return DummyMoveComp->GetHalfExtentForZone(ForZone);
}

const UAdvKitCharacterMovementComponent* UAdvKitTransitionBuilderModule::GetCharacterMovementComponent(TSubclassOf<AAdvKitCharacter> ForCharacterClass) const
{
	auto DefaultCharacter = GetDefaultCharacterObject(ForCharacterClass);
	check(DefaultCharacter);

	return DefaultCharacter->GetAdvMovementComponent();
}

const FAdvKitMovementProperties* UAdvKitTransitionBuilderModule::GetCharacterMovementProperties(TSubclassOf<AAdvKitCharacter> ForCharacterClass) const
{
	auto DefaultCharacter = GetDefaultCharacterObject(ForCharacterClass);
	check(DefaultCharacter);

	return DefaultCharacter->GetAdvMovementProperties();
}

const FAdvKitMovementProperties& UAdvKitTransitionBuilderModule::GetCharacterMovementProperties_BP(TSubclassOf<AAdvKitCharacter> ForCharacterClass) const
{
	auto DefaultCharacter = GetDefaultCharacterObject(ForCharacterClass);
	check(DefaultCharacter);

	return *DefaultCharacter->GetAdvMovementProperties();
}

AAdvKitCharacter* UAdvKitTransitionBuilderModule::GetDefaultCharacterObject(TSubclassOf<AAdvKitCharacter> ForCharacterClass) const
{
	check(ForCharacterClass);

	//Not sure if this can ever happen, but I am a pessimist :P
	AAdvKitCharacter* DefaultCharacter = ForCharacterClass.GetDefaultObject();
	if (!DefaultCharacter)
	{
		UE_LOG(LogAdvKit, Error, TEXT("UAdvKitTransitionBuilderModule::GetDefaultCharacterObject Given character class does not have default object!"));
		return nullptr;
	}

	return DefaultCharacter;
}


bool UAdvKitTransitionBuilderModule::OverlapForCloseZones(class AAdvKitZone* ToZone, FVector OverlapStart, FVector OverlapEnd, float OverlapRadius, TArray<AAdvKitZone*>& OutOtherZones)
{
	static const float Half = 0.5f;

	//Cannot overlap without the zone or the engine
	if (!ToZone)
	{
		UE_LOG(LogAdvKit,Error,TEXT("Cannot create transition for nullptr zone!"))
		return false;
	}

	//Get world for overlap check
	UWorld* World = GEngine->GetWorldFromContextObject(ToZone);
	if (!World)
	{
		UE_LOG(LogAdvKit, Error, TEXT("Cannot create transitions without World!"))
		return false;
	}

	//Params for overlap
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ToZone);
	FCollisionObjectQueryParams ObjectQueryParams;

	//Create overlap box
	FVector LocalStart = ToZone->GetTransform().InverseTransformPositionNoScale(OverlapStart);
	FVector LocalEnd = ToZone->GetTransform().InverseTransformPositionNoScale(OverlapEnd);
	FVector BoxExtent = (LocalEnd - LocalStart)*Half + FVector(1, 1, 1)*OverlapRadius;
	FVector BoxLocation = FMath::Lerp<FVector>(OverlapStart, OverlapEnd, Half);

	//Execute overlap
	if (!World->OverlapMultiByObjectType(OverlapResults, BoxLocation, ToZone->GetActorQuat(), ObjectQueryParams, FCollisionShape::MakeBox(BoxExtent), QueryParams))
	{
		//Nothing was found
		return false;
	}

	//Filter zones from overlap results
	for (FOverlapResult Result : OverlapResults)
	{
		if (!Result.Actor.IsValid())
		{
			//Only actors can be zones
			continue;
		}

		if(!Result.Actor->IsA<AAdvKitZone>())
		{
			//Not a zone
			continue;
		}

		//Add zone to result
		OutOtherZones.AddUnique(Cast<AAdvKitZone>(Result.Actor.Get()));
	}

	//Return if any zones were found
	return (OutOtherZones.Num() != 0);
}

TArray<UAdvKitTransitionComponent*> UAdvKitTransitionBuilderModule::GetPreExistingTransitions(FString Id, AAdvKitZone* FromZone, AAdvKitZone* ToZone, TSubclassOf<AAdvKitCharacter> ForCharacterClass) const
{
	TArray<UAdvKitTransitionComponent*> OutArray;

	auto Transitions = FromZone->GetComponentsByClass(UAdvKitTransitionComponent::StaticClass());

	//Go through all transitions
	for (auto Component : Transitions)
	{
		auto TransitionComponent = Cast<UAdvKitTransitionComponent>(Component);

		//Check if usable by character
		if (ForCharacterClass != TransitionComponent->UseableBy)
		{
			continue;
		}

		if (TransitionComponent->TransitionId != Id)
		{
			continue;
		}

		//Add all transitions to the result
		OutArray.Add(TransitionComponent);
	}

	return OutArray;

}


TArray<UAdvKitTransitionComponent*> UAdvKitTransitionBuilderModule::GetPreExistingTransitions(class AAdvKitZone* FromZone, class AAdvKitZone* ToZone) const
{
	TArray<UAdvKitTransitionComponent*> OutArray;

	auto Transitions = FromZone->GetComponentsByClass(UAdvKitTransitionComponent::StaticClass());

	//Go through all transitions
	for (auto Component : Transitions)
	{
		auto TransitionComponent = Cast<UAdvKitTransitionComponent>(Component);

		if (TransitionComponent->TargetZone != ToZone)
		{
			continue;
		}

		if (TransitionComponent->CreatedByBuildModule != this)
		{
			continue;
		}

		//Add all transitions to the result
		OutArray.Add(TransitionComponent);
	}

	return OutArray;

}

TArray<UAdvKitTransitionComponent*> UAdvKitTransitionBuilderModule::GetPreExistingTransitions(class AAdvKitZone* FromZone) const
{
	TArray<UAdvKitTransitionComponent*> OutArray;

	auto Transitions = FromZone->GetComponentsByClass(UAdvKitTransitionComponent::StaticClass());

	//Go through all transitions
	for (auto Component : Transitions)
	{
		auto TransitionComponent = Cast<UAdvKitTransitionComponent>(Component);
		if (/*TransitionComponent->CreatedByBuildModule && */TransitionComponent->CreatedByBuildModule != this)
		{
			continue;
		}

		//Add all transitions to the result
		OutArray.Add(TransitionComponent);
	}

	return OutArray;

}

UAdvKitTransitionComponentPoint* UAdvKitTransitionBuilderModule::CreateTransitionPoint(FString Id, AAdvKitZone* SourceZone, const FVector& WorldSourceLocation, const FVector& WorldTargetLocation, TSubclassOf<AAdvKitCharacter> ForCharacter, EAdvKitMovementMode MovementMode, AAdvKitZone* TargetZone, uint8 CustomMovementMode)
{
	bool bRenameTransition = true;
	UAdvKitTransitionComponentPoint* Transition = nullptr;

	FString UniqueId = MakeUniqueId(Id, SourceZone, TargetZone);

	auto ExistingTransitions = GetPreExistingTransitions(UniqueId, SourceZone, TargetZone, ForCharacter);
	if (ExistingTransitions.Num() > 0)
	{
		Transition = Cast<UAdvKitTransitionComponentPoint>(ExistingTransitions[0]);
		if (Transition)
		{
			bRenameTransition = false;
			Transition->Init(ForCharacter, WorldSourceLocation, WorldTargetLocation, MovementMode, TargetZone, CustomMovementMode);
		}
	}
	
	if (!Transition)
	{
		//UE_LOG(LogAdvKit, Warning, TEXT("No pre-existing transition for %s"), *UniqueId);
		Transition = SourceZone->RegisterTransition(WorldSourceLocation, WorldTargetLocation, ForCharacter, MovementMode, TargetZone, CustomMovementMode);
	}

	if (!Transition)
	{
		return nullptr;
	}

	CreatedOrUpdatedTransitions.AddUnique(Transition);
	AffectedZones.AddUnique(TargetZone);
	Transition->CreatedByBuildModule = this;
	Transition->TransitionId = UniqueId;

	if (bRenameTransition)
	{
		FString NewName = Id;
		if (TargetZone)
		{
			NewName += Id + "_to_" + TargetZone->GetName();
		}

		NewName = MakeUniqueObjectName(GetOuter(), UAdvKitTransitionComponentPoint::StaticClass(), FName(*NewName)).ToString();
		Transition->Rename(*NewName, GetOuter());
	}

	return Transition;
}

UAdvKitTransitionComponentArea* UAdvKitTransitionBuilderModule::CreateTransitionArea(FString Id, AAdvKitZone* SourceZone, const FVector& MinWorldSourceLocation, const FVector& MinWorldTargetLocation, const FVector& MaxWorldSourceLocation, const FVector& MaxWorldTargetLocation, TSubclassOf<AAdvKitCharacter> ForCharacter, EAdvKitMovementMode MovementMode, AAdvKitZone* TargetZone, uint8 CustomMovementMode)
{
	bool bRenameTransition = true;
	UAdvKitTransitionComponentArea* Transition = nullptr;
	FString UniqueId = MakeUniqueId(Id, SourceZone, TargetZone);

	auto ExistingTransitions = GetPreExistingTransitions(UniqueId, SourceZone, TargetZone, ForCharacter);
	if (ExistingTransitions.Num() > 0)
	{
		Transition = Cast<UAdvKitTransitionComponentArea>(ExistingTransitions[0]);
		if (Transition)
		{
			bRenameTransition = false;
			Transition->Init(ForCharacter, MinWorldSourceLocation, MinWorldTargetLocation, MaxWorldSourceLocation, MaxWorldTargetLocation, MovementMode, TargetZone, CustomMovementMode);
		}
	}

	if (!Transition)
	{
		Transition = SourceZone->RegisterTransitionArea(MinWorldSourceLocation, MinWorldTargetLocation, MaxWorldSourceLocation, MaxWorldTargetLocation, ForCharacter, MovementMode, TargetZone, CustomMovementMode);
	}

	if (!Transition)
	{
		return nullptr;
	}

	CreatedOrUpdatedTransitions.AddUnique(Transition);
	AffectedZones.AddUnique(TargetZone);
	Transition->CreatedByBuildModule = this;
	Transition->TransitionId = UniqueId;

	if (bRenameTransition)
	{
		FString NewName = Id;
		if (TargetZone)
		{
			NewName += "_to_" + TargetZone->GetName();
		}

		NewName = MakeUniqueObjectName(GetOuter(), UAdvKitTransitionComponentPoint::StaticClass(), FName(*NewName)).ToString();
		Transition->Rename(*NewName, GetOuter());
	}

	return Transition;
}

FString UAdvKitTransitionBuilderModule::MakeUniqueId(FString BaseId, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone)
{
	FString NewId = GetName() + "_" + BaseId;

	//if (SourceZone)
	//{
	//	NewId += "_From_" + SourceZone->GetName();
	//}

	if (TargetZone)
	{
		NewId += "_To_" + TargetZone->GetName();
	}

	return NewId;
}
