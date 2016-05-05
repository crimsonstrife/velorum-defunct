// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "Actions/AdvKitCharacterAction_CharacterModifier.h"
#include "AdvKitCA_Mod_Item.generated.h"

/**
 * @brief Changes the character's equipped item
 */
UCLASS(DisplayName="Modify Item")
class ADVKITRUNTIME_API UAdvKitCA_Mod_Item : public UAdvKitCharacterAction_CharacterModifier
{
	GENERATED_BODY()

public:

	/** The new weapon to equip (or unequip current when NULL). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (EditCondition = "bChangeEquippedItem", Tooltip = "The new item to equip (or unequip current if value is None).", DisplayName = "Item to equip"))
	TSubclassOf<class AAdvKitInventoryItem> ItemClassToEquip;

	/** Whether or not the item change should be instant. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (EditCondition = "bChangeEquippedItem", Tooltip = "The new item to equip (or unequip current if value is None).", DisplayName = "Change Item instantly"))
	bool bInstantItemChange;

	//Begin UAdvKitCharacterAction_CharacterModifier Interface
	virtual void ApplyToCharacter(class AAdvKitCharacter* ToCharacter, class UAdvKitCharacterMovementComponent* ToMovementComponent);
	//End UAdvKitCharacterAction_CharacterModifier Interface
};
