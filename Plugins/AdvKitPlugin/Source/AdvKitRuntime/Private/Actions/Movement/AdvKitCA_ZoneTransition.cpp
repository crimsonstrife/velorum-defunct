// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/Movement/AdvKitCA_ZoneTransition.h"
#include "Actions/Movement/AdvKitCA_ZoneTransition_Arguments.h"
#include "Actions/Modifiers/AdvKitCA_Mod_MovementMode.h"

#include "Environment/AdvKitZoneLocation.h"
#include "Environment/AdvKitZone.h"
#include "Environment/AdvKitTransitionComponent.h"

#include "Player/AdvKitCharacter.h"

UAdvKitCA_ZoneTransition::UAdvKitCA_ZoneTransition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAdvKitCA_ZoneTransition::OnMontageEnded_Implementation(UAnimMontage *EndedMontage, bool bInterrupted)
{
	if (EndedMontage != CurrentAnimMontage)
	{
		return;
	}

	if (!IsActive())
	{
		//Already ended
		return;
	}

	CurrentAnimMontage = nullptr;
	Stop();
}

bool UAdvKitCA_ZoneTransition::CanBeInterruptedBy_Implementation(const UAdvKitCharacterAction* Other) const
{
	return false;
}


FAdvKitActionResponse UAdvKitCA_ZoneTransition::Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments, UAdvKitCharacterAction* InterruptedOther)
{
	//Cannot do anything without arguments
	CurrentArguments = Cast<UAdvKitCA_ZoneTransition_Arguments>(Arguments);
	if (!CurrentArguments)
	{
		return Failed();
	}

	FAdvKitActionResponse Response = Super::Start_Implementation(Arguments, InterruptedOther);

	CurrentAnimMontage = GetMontageToPlay();
	if (!IsValid(CurrentAnimMontage))
	{
		return Stop();
	}

	PlayAnimMontage(CurrentAnimMontage);
	return Response;
}

const UAdvKitCA_ZoneTransition_Arguments * UAdvKitCA_ZoneTransition::GetCurrentArguments() const
{
	return CurrentArguments;
}

UAnimMontage* UAdvKitCA_ZoneTransition::GetMontageToPlay_Implementation()
{
	return nullptr;
}


class UAdvKitCharacterAction_Arguments* UAdvKitCA_ZoneTransition::MakeArguments(AAdvKitZone* Zone)
{
	UAdvKitCA_ZoneTransition_Arguments* Arguments = NewObject<UAdvKitCA_ZoneTransition_Arguments>();
	Arguments->Zone = Zone;
	return Arguments;
}

UAdvKitCharacterAction_Arguments* UAdvKitCA_ZoneTransition::MakeArguments(UAdvKitTransitionComponent* Transition)
{
	UAdvKitCA_ZoneTransition_Arguments* Arguments = NewObject<UAdvKitCA_ZoneTransition_Arguments>();
	Arguments->Transition = Transition;
	if (Transition)
	{
		Arguments->Zone = Transition->TargetZone.Get();
	}
	return Arguments;
}
