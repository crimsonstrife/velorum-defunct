// Copyright 2015 Pascal Krabbe

#pragma once

#pragma once
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_CharacterModifier.generated.h"

class UAdvKitCharacterAction_CharacterModifier;

UCLASS(meta = (DisplayName = "Modify Character"))
class UAnimNotify_CharacterModifier : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	/** Configurations to set the CharacterOwner to when the action starts */
	UPROPERTY(Instanced, EditAnywhere, BlueprintReadOnly, Category = "Action", meta = (Tooltip = "Configurations that make changes to the character."))
	TArray<class UAdvKitCharacterAction_CharacterModifier*> Modifiers;
};