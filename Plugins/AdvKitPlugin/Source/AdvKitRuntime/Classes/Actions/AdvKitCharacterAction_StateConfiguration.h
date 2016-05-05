// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "AdvKitCharacterAction_StateConfiguration.generated.h"

/**
 * @brief Every action can change the state of their respective owner character. This class works as
 * an utility to encapsulate configuration parameters.
 */
UCLASS()
class ADVKITRUNTIME_API UAdvKitCharacterAction_StateConfiguration : public UObject
{
	GENERATED_BODY()
public: 
	/**
	 * Constructor
	 */
	UAdvKitCharacterAction_StateConfiguration();

	/** Whether or not to change the collision. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (Tooltip = "Whether or not to change the collision."))
	bool bChangeCollision;

	/** Enable or disable collision. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (EditCondition = "bChangeCollision", Tooltip = "Enable or disable collision."))
	bool bEnableCollision;

	/** Whether or not to change the movement mode. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (Tooltip = "Whether or not to change the movement mode."))
	bool bChangeMovementMode;

	/** The new movement mode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (EditCondition = "bChangeMovementMode", Tooltip = "The new movement mode."))
	EAdvKitMovementMode MovementMode;

	/** The new custom movement mode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (EditCondition = "bChangeMovementMode", Tooltip = "The new custom movement mode."))
	uint8 CustomMovementMode;

	/** Whether or not to equip an item (or unequip current if ItemClassToEquip is NULL). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (Tooltip = "Whether or not to change the equipped item (or unequip current in case 'Item to equip' is None)."))
	bool bChangeEquippedItem;

	/** The new weapon to equip (or unequip current when NULL). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (EditCondition = "bChangeEquippedItem", Tooltip = "The new item to equip (or unequip current if value is None).", DisplayName = "Item to equip"))
	TSubclassOf<class AAdvKitInventoryItem> ItemClassToEquip;

	/** Whether or not the item change should be instant. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (EditCondition = "bChangeEquippedItem", Tooltip = "The new item to equip (or unequip current if value is None).", DisplayName = "Change Item instantly"))
	bool bInstantItemChange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	bool bChangeYawRotationBind;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration", meta = (EditCondition = "bChangeYawRotationBind"))
	bool bUseControllerRotationYawInFirstPerson;

	/**
	 * Apply this configuration to a character and movement component.
	 * 
	 * @param	ToCharacter				The character to apply the configuration to.
	 * @param	ToMovementComponent		The movement component to apply the configuration to.
	 */
	virtual void ApplyToCharacter(class AAdvKitCharacter* ToCharacter, class UAdvKitCharacterMovementComponent* ToMovementComponent);
};
