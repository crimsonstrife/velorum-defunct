// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/AnimNotifyState_TransitionCorrection.h"

#include "Player/AdvKitCharacter.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "Actions/Movement/AdvKitCA_ZoneTransition.h"
#include "Actions/Movement/AdvKitCA_ZoneTransition_Arguments.h"
#include "Environment/AdvKitZoneLocation.h"


FVector FTransitionLocation::GetLocation(UAdvKitCharacterMovementComponent* MovementComponent) const
{
	if (!ZoneLocation || !MovementComponent)
	{
		return Location;
	}

	FVector OffsetFromZone = MovementComponent->GetDesiredOffsetFromZone(ZoneLocation->Zone, ZoneLocation);
	return ZoneLocation->GetGlobalPosition() + OffsetFromZone;
}

FRotator FTransitionLocation::GetRotation(UAdvKitCharacterMovementComponent* MovementComponent) const
{
	if (!ZoneLocation || !MovementComponent)
	{
		return Rotation;
	}

	return MovementComponent->GetDesiredRotationInZone(ZoneLocation->Zone, ZoneLocation);
}

void FTransitionLocation::Clear()
{
	ZoneLocation = nullptr;
	Location = FVector::ZeroVector;
	Rotation = FRotator::ZeroRotator;
}



AAdvKitCharacter* UAnimNotifyState_TransitionCorrection::GetOwningCharacter(USkeletalMeshComponent * MeshComp) const
{
	return Cast<AAdvKitCharacter>(MeshComp->GetOwner());
}

UAdvKitCharacterMovementComponent* UAnimNotifyState_TransitionCorrection::GetOwningMovementComponent(USkeletalMeshComponent * MeshComp) const
{
	auto Character = GetOwningCharacter(MeshComp);
	if (!Character)
	{
		return nullptr;
	}

	return Character->GetAdvMovementComponent();
}

const UAdvKitCA_ZoneTransition_Arguments* UAnimNotifyState_TransitionCorrection::GetCurrentArguments(USkeletalMeshComponent * MeshComp) const
{
	auto Character = GetOwningCharacter(MeshComp);
	if (!Character)
	{
		return nullptr;
	}

	auto Action = Cast<UAdvKitCA_ZoneTransition>(Character->GetActiveAction());
	if (!Action)
	{
		return nullptr;
	}
	
	return Action->GetCurrentArguments();
}

void UAnimNotifyState_TransitionCorrection::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	PreviousLocation.Clear();
	NextLocation.Clear();

	auto CharacterOwner = GetOwningCharacter(MeshComp);
	if (!CharacterOwner)
	{
		return;
	}

	auto MovementComponent = GetOwningMovementComponent(MeshComp);
	if (!MovementComponent)
	{
		return;
	}

	auto CurrentArguments = GetCurrentArguments(MeshComp);
	if (!CurrentArguments)
	{
		return;
	}

	FVector TranslationOffset = FVector::ZeroVector;
	FQuat RotationOffset = FQuat::Identity;

	auto Montage = Cast<UAnimMontage>(Animation);
	if (Montage)
	{
		float Start = 0;

		auto AnimInst = MeshComp->GetAnimInstance();
		if (AnimInst)
		{
			Start = AnimInst->Montage_GetPosition(Montage);
		}
		
		float End = Start + TotalDuration;

		auto RootMotion = Montage->ExtractRootMotionFromTrackRange(Start, End);
		RootMotion = MeshComp->ConvertLocalRootMotionToWorld(RootMotion);
			
		TranslationOffset = RootMotion.GetTranslation();
		RotationOffset = RootMotion.GetRotation();
	}

	PreviousLocation.ZoneLocation = CharacterOwner->GetZoneLocation();
	PreviousLocation.Location = CharacterOwner->GetActorLocation();
	PreviousLocation.Rotation = CharacterOwner->GetActorRotation();

	LastLocationAlpha = 0;
	LastRotationAlpha = 0;
	TranslationDifference = FVector::ZeroVector;
	RotationDifference = FRotator::ZeroRotator;

	FQuat RotationAfterMontage = CharacterOwner->GetActorQuat() * RotationOffset;
	FVector LocationAfterMontage = CharacterOwner->GetActorLocation() + TranslationOffset;

	NextLocation.Location = LocationAfterMontage;
	NextLocation.Rotation = RotationAfterMontage.Rotator();

	//Calculate the necessary delta for the transition
	if (CurrentArguments->Zone)
	{
		FVector HalfExtent = MovementComponent->GetHalfExtentForZone(CurrentArguments->Zone);
		
		NextLocation.ZoneLocation = CurrentArguments->Zone->GetClosestZoneLocation(
			LocationAfterMontage,
			HalfExtent,
			RotationAfterMontage.Rotator());

		//Find out where exactly the character will be positioned after the transition
		FVector OffsetFromZone = MovementComponent->GetDesiredOffsetFromZone(CurrentArguments->Zone, NextLocation.ZoneLocation);
		NextLocation.Location = NextLocation.ZoneLocation->GetGlobalPosition() + OffsetFromZone;
		TranslationDifference = NextLocation.Location - LocationAfterMontage;

		//Calculate delta rotation
		NextLocation.Rotation = MovementComponent->GetDesiredRotationInZone(CurrentArguments->Zone, NextLocation.ZoneLocation);
		RotationDifference = (NextLocation.Rotation.Quaternion() * RotationAfterMontage.Inverse()).Rotator();
		return;
	}

	//Only transition was given, but no zone, so it must be an exit transition
	if (CurrentArguments->Transition)
	{
		NextLocation.ZoneLocation = nullptr;
		NextLocation.Location = CurrentArguments->Transition->GetClosestTargetPosition(CharacterOwner->GetActorLocation());
		NextLocation.Rotation = RotationAfterMontage.Rotator();
		TranslationDifference = NextLocation.Location - LocationAfterMontage;
		return;
	}
}

void UAnimNotifyState_TransitionCorrection::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	auto CurrentArguments = GetCurrentArguments(MeshComp);
	if (!CurrentArguments)
	{
		return;
	}

	float LocationAlpha = 0;
	float RotationAlpha = 0;

	UAnimInstance * AnimInst = MeshComp->GetAnimInstance();
	if(AnimInst)
	{
		LocationAlpha = AnimInst->GetCurveValue(FName("Transition Location Correction"));
		RotationAlpha = AnimInst->GetCurveValue(FName("Transition Rotation Correction"));
	}

	if (!bAbsoluteTransitionCorrection)
	{
		float LocationDifAlpha = LocationAlpha - LastLocationAlpha;
		float RotationDifAlpha = RotationAlpha - LastRotationAlpha;

		LastLocationAlpha = LocationAlpha;
		LastRotationAlpha = RotationAlpha;

		GetOwningCharacter(MeshComp)->AddActorWorldOffset(TranslationDifference * LocationDifAlpha);
		GetOwningCharacter(MeshComp)->AddActorWorldRotation(RotationDifference * RotationDifAlpha);
	}
	else
	{
		auto CharacterOwner = GetOwningCharacter(MeshComp);
		if (!CharacterOwner)
		{
			return;
		}

		auto MovementComponent = GetOwningMovementComponent(MeshComp);
		if (!MovementComponent)
		{
			return;
		}

		FVector TransitionLocation = FMath::Lerp<FVector>(PreviousLocation.GetLocation(MovementComponent), NextLocation.GetLocation(MovementComponent), LocationAlpha);
		FQuat TransitionRotation = FQuat::Slerp(
			PreviousLocation.GetRotation(MovementComponent).Quaternion(),
			NextLocation.GetRotation(MovementComponent).Quaternion(), 
			LocationAlpha);
		
		float EnforcementAlpha = AnimInst->GetCurveValue(FName("Transition Correction Enforcement"));
		CharacterOwner->SetActorLocation(FMath::Lerp<FVector>(CharacterOwner->GetActorLocation(), TransitionLocation, EnforcementAlpha));
		CharacterOwner->SetActorRotation(
			FQuat::Slerp(
				CharacterOwner->GetActorRotation().Quaternion(),
				TransitionRotation, EnforcementAlpha));
	}
}

void UAnimNotifyState_TransitionCorrection::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	PreviousLocation.Clear();
	NextLocation.Clear();
}
