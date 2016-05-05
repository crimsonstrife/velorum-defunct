// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/Movement/AdvKitCA_ZoneTransition_NoAnim.h"

UAdvKitCA_ZoneTransition_NoAnim::UAdvKitCA_ZoneTransition_NoAnim(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = false;

	auto MovementModeMod = ObjectInitializer.CreateDefaultSubobject<UAdvKitCA_Mod_Auto_MovementMode>(this, "DefaultMovementMod");
	StopModifiers.Add(MovementModeMod);

	bSetTargetZoneBaseAtStart = true;
	ApplyZoneOnStop = true;

	//Add default keys to transition curve forming a straight line
	FKeyHandle Handle1;
	TransitionCurve.GetRichCurve()->AddKey(0, 0, false, Handle1);
	TransitionCurve.GetRichCurve()->GetKey(Handle1).TangentMode = ERichCurveTangentMode::RCTM_Auto;
	TransitionCurve.GetRichCurve()->GetKey(Handle1).InterpMode = ERichCurveInterpMode::RCIM_Cubic;

	FKeyHandle Handle2;
	TransitionCurve.GetRichCurve()->AddKey(1, 1, false, Handle2);
	TransitionCurve.GetRichCurve()->GetKey(Handle2).TangentMode = ERichCurveTangentMode::RCTM_Auto;
	TransitionCurve.GetRichCurve()->GetKey(Handle2).InterpMode = ERichCurveInterpMode::RCIM_Cubic;

	TransitionCurve.GetRichCurve()->AutoSetTangents();
}

void UAdvKitCA_ZoneTransition_NoAnim::TickComponent(float DeltaSeconds, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

	//Add alpha time to transition time
	CurrentTransitionTime += DeltaSeconds;

	//Get new value
	float AlphaValue = TransitionCurve.GetRichCurve()->Eval(CurrentTransitionTime);

	//Apply orientation delta
	FVector TransitionLocation = FMath::Lerp<FVector>(PreviousLocation.GetLocation(MovementComponent), NextLocation.GetLocation(MovementComponent), AlphaValue);
	FQuat TransitionRotation = FQuat::Slerp(
		PreviousLocation.GetRotation(MovementComponent).Quaternion(),
		NextLocation.GetRotation(MovementComponent).Quaternion(),
		AlphaValue);

	CharacterOwner->SetActorLocation(TransitionLocation);
	CharacterOwner->SetActorRotation(TransitionRotation);

	//No animation means the transition curve has full controll over ending the action.
	if (CurrentTransitionTime > TransitionCurve.GetRichCurve()->GetLastKey().Time)
	{
		Stop();
	}
}

FAdvKitActionResponse UAdvKitCA_ZoneTransition_NoAnim::Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments, UAdvKitCharacterAction* InterruptedOther)
{
	PreviousLocation.Clear();
	NextLocation.Clear();

	//Cannot do anything without arguments
	CurrentArguments = Cast<UAdvKitCA_ZoneTransition_Arguments>(Arguments);
	if (!CurrentArguments)
	{
		return Failed();
	}

	//Set the new zone if required
	if (ApplyZoneOnStart)
	{
		CharacterOwner->SetZone(CurrentArguments ? CurrentArguments->Zone : nullptr);
	}

	SetActive(true);
	ApplyStartModifiers();
	FAdvKitActionResponse Response = Succeeded();// Super::Super::Start_Implementation(Arguments, InterruptedOther);

	CurrentTransitionTime = 0;

	PreviousLocation.ZoneLocation = CharacterOwner->GetZoneLocation();
	PreviousLocation.Location = CharacterOwner->GetActorLocation();
	PreviousLocation.Rotation = CharacterOwner->GetActorRotation();

	NextLocation.Location = PreviousLocation.Location;
	NextLocation.Rotation = PreviousLocation.Rotation;

	//Calculate the necessary delta for the transition
	if (CurrentArguments->Zone)
	{
		FVector HalfExtent = MovementComponent->GetHalfExtentForZone(CurrentArguments->Zone);

		NextLocation.ZoneLocation = CurrentArguments->Zone->GetClosestZoneLocation(
			PreviousLocation.Location,
			HalfExtent,
			PreviousLocation.Rotation);

		//Find out where exactly the character will be positioned after the transition
		FVector OffsetFromZone = MovementComponent->GetDesiredOffsetFromZone(CurrentArguments->Zone, NextLocation.ZoneLocation);
		NextLocation.Location = NextLocation.ZoneLocation->GetGlobalPosition() + OffsetFromZone;
		NextLocation.Rotation = MovementComponent->GetDesiredRotationInZone(CurrentArguments->Zone, NextLocation.ZoneLocation);

		if (bSetTargetZoneBaseAtStart)
		{
			CharacterOwner->SetBase(Cast<UPrimitiveComponent>(CurrentArguments->Zone->GetRootComponent()));
		}

		return Response;
	}

	//Only transition was given, but no zone, so it must be an exit transition
	if (CurrentArguments->Transition)
	{
		NextLocation.ZoneLocation = nullptr;
		NextLocation.Location = CurrentArguments->Transition->GetClosestTargetPosition(CharacterOwner->GetActorLocation());
		return Response;
	}

	return Response;
}

FAdvKitActionResponse UAdvKitCA_ZoneTransition_NoAnim::Stop_Implementation(const UAdvKitCharacterAction* InterruptedBy)
{
	SetActive(false);
	auto Result = Super::Stop_Implementation(InterruptedBy);


	if (ApplyZoneOnStop)
	{
		CharacterOwner->SetZone(CurrentArguments ? CurrentArguments->Zone : nullptr);
	}

	PreviousLocation.Clear();
	NextLocation.Clear();

	return Result;
}

