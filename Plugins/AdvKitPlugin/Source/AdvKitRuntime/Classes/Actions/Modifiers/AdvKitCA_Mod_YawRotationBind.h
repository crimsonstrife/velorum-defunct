// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "Actions/AdvKitCharacterAction_CharacterModifier.h"
#include "AdvKitCA_Mod_YawRotationBind.generated.h"

/**
 * @brief Changes the character's rotation binding
 */
UCLASS(DisplayName="Modify Controller Yaw Usage")
class ADVKITRUNTIME_API UAdvKitCA_Mod_YawRotationBind : public UAdvKitCharacterAction_CharacterModifier
{
	GENERATED_BODY()

public:

	/** Whether or not to use the controller rotation when in first person. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (EditCondition = "bChangeYawRotationBind"))
	bool bUseControllerRotationYawInFirstPerson;

	//Begin UAdvKitCharacterAction_CharacterModifier Interface
	virtual void ApplyToCharacter(class AAdvKitCharacter* ToCharacter, class UAdvKitCharacterMovementComponent* ToMovementComponent);
	//End UAdvKitCharacterAction_CharacterModifier Interface
};
