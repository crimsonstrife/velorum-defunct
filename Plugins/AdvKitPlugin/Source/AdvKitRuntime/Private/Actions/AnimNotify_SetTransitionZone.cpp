// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/AnimNotify_SetTransitionZone.h"
#include "Actions/AdvKitCharacterAction_CharacterModifier.h"

UAnimNotify_SetTransitionZone::UAnimNotify_SetTransitionZone()
	: Super()
{
	bSnapToZone = true;
}

void UAnimNotify_SetTransitionZone::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	auto Character = Cast<AAdvKitCharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	auto Action = Cast<UAdvKitCA_ZoneTransition>(Character->GetActiveAction());
	if (!Action)
	{
		return;
	}

	auto Args = Action->GetCurrentArguments();
	if (!Args)
	{
		return;
	}

	Character->SetZone(Args->Zone, bSnapToZone);
}
