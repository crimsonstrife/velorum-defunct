// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/Movement/AdvKitCA_ZoneTransition_Directional.h"
#include "Actions/Movement/AdvKitCA_ZoneTransition_Arguments.h"
#include "Actions/Modifiers/AdvKitCA_Mod_MovementMode.h"

#include "Environment/AdvKitZoneLocation.h"
#include "Environment/AdvKitZone.h"
#include "Environment/AdvKitTransitionComponent.h"

#include "Player/AdvKitCharacter.h"

UAnimMontage* UAdvKitCA_ZoneTransition_Directional::GetMontageToPlay_Implementation()
{
	auto Args = GetCurrentArguments();
	if (!Args)
	{
		return nullptr;
	}

	FVector CharacterLocation = CharacterOwner->GetActorLocation();

	FVector WorldDirection = FVector::ZeroVector;
	if (Args->Transition)
	{
		FVector Source = Args->Transition->GetClosestSourcePosition(CharacterLocation);
		FVector Target = Args->Transition->GetClosestTargetPosition(CharacterLocation);

		WorldDirection = (Target - Source).GetSafeNormal();
	}
	else if(Args->Zone)
	{
		FVector Target = Args->Zone->ConstrainPositionToZone(CharacterLocation,FVector::ZeroVector);
		WorldDirection = (Target - CharacterLocation).GetSafeNormal();
	}
	else
	{
		return nullptr;
	}

	float MaxDot = 0;
	int32 BestIndex = INDEX_NONE;
	FVector LocalDirection = CharacterOwner->GetTransform().InverseTransformVector(WorldDirection).GetSafeNormal();

	for (int32 i = 0; i < Montages.Num(); i++)
	{
		float NewDot = FVector::DotProduct(Montages[i].Direction.GetSafeNormal(), LocalDirection);
		if (BestIndex == -1 || NewDot > MaxDot)
		{
			MaxDot = NewDot;
			BestIndex = i;
		}
	}

	if (BestIndex == INDEX_NONE)
	{
		return nullptr;
	}

	return Montages[BestIndex].Montage;
}
