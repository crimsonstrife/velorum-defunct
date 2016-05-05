// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "Actions/AdvKitCharacterAction_CharacterModifier.h"
#include "AdvKitCA_Mod_OrientationToZone.generated.h"

/**
 * @brief Changes the character's transform relative to the zone it transitions to/from
 */
UCLASS(DisplayName="Orient To Zone")
class ADVKITRUNTIME_API UAdvKitCA_Mod_OrientationToZone : public UAdvKitCharacterAction_CharacterModifier
{
	GENERATED_BODY()

protected:
	/** Whether to set a relative location */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	bool bSetLocation;

	/** Relative location to set */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (EditCondition = bSetLocation))
	FVector Location;

	/** Whether to set a relative rotation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	bool bSetRotation;

	/** Relative rotation to set */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (EditCondition = bSetRotation))
	FRotator Rotation;

public:

	/** Constructor */
	UAdvKitCA_Mod_OrientationToZone(const FObjectInitializer& ObjectInitializer);

	//Begin UAdvKitCharacterAction_CharacterModifier Interface
	virtual void ApplyToCharacter(class AAdvKitCharacter* ToCharacter, class UAdvKitCharacterMovementComponent* ToMovementComponent);
	//End UAdvKitCharacterAction_CharacterModifier Interface
};
