// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/AnimNotify_CharacterModifier.h"
#include "Actions/AdvKitCharacterAction_CharacterModifier.h"

void UAnimNotify_CharacterModifier::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	auto Character = Cast<AAdvKitCharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	auto MovementComponent = Character->GetAdvMovementComponent();
	if (!MovementComponent)
	{
		return;
	}

	for (auto Modifier : Modifiers)
	{
		if (!IsValid(Modifier))
		{
			continue;
		}

		Modifier->ApplyToCharacter(Character, MovementComponent);
	}
}
