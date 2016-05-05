// Copyright 2015 Pascal Krabbe

#pragma once

#pragma once
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SetTransitionZone.generated.h"

class UAdvKitCharacterAction_CharacterModifier;

UCLASS(meta = (DisplayName = "Set Zone"))
class UAnimNotify_SetTransitionZone : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category="Transition")
	bool bSnapToZone;

public:
	UAnimNotify_SetTransitionZone();
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};