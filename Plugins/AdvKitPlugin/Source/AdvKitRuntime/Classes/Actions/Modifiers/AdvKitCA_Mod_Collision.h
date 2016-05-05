// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "Actions/AdvKitCharacterAction_CharacterModifier.h"
#include "AdvKitCA_Mod_Collision.generated.h"

/**
 * @brief Helper struct to configure responses to channels.
 */
USTRUCT(BlueprintType)
struct FResponseConfig
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> Channel;

	UPROPERTY(EditAnywhere)
		TEnumAsByte<ECollisionResponse> Response;
};

/**
 * @brief Changes the characters collision in an action
 */
UCLASS(DisplayName="Modify Collision")
class ADVKITRUNTIME_API UAdvKitCA_Mod_Collision : public UAdvKitCharacterAction_CharacterModifier
{
	GENERATED_BODY()

public:

	UAdvKitCA_Mod_Collision();

	/** Changes to apply to character collision reponses. */
	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (Tooltip = "Changes to make regarding the character capsule's collision response."))
	TArray<FResponseConfig> ResponseChanges;

	/** Enable or disable collision. */
	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (Tooltip = "Toggle whether or not to change collision."))
	bool bChangeCollision;

	/** Enable or disable collision. */
	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (EditCondition = "bChangeCollision", Tooltip = "Enable or disable collision."))
	bool bEnableCollision;

	//Begin UAdvKitCharacterAction_CharacterModifier Interface
	virtual void ApplyToCharacter(class AAdvKitCharacter* ToCharacter, class UAdvKitCharacterMovementComponent* ToMovementComponent);
	//End UAdvKitCharacterAction_CharacterModifier Interface
};
