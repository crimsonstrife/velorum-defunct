// Copyright 2015 Pascal Krabbe

#pragma once

#include "GameFramework/Actor.h"
#include "AdvKitTypes.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "AdvKitZone.generated.h"

class UAdvKitTransitionComponentPoint;
class UAdvKitTransitionComponentArea;
class UAdvKitZoneLocation;
class UAdvKitTransitionBuilderModule;
class UAdvKitManualTransition;

/**
 * @brief Zones are special movement objects that a character can use for specific movement modes. 
 * 
 * A zone always restricts movement within a specified region, e.g. a line or rectangle. Zones can 
 * also have predefined transition points between them.
 */
UCLASS(abstract)
class ADVKITRUNTIME_API AAdvKitZone : public AActor
{
	GENERATED_BODY()

public:
//protected:
	/** Movement mode to be used in this zone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	EAdvKitMovementMode ZonePhysics;

	/** Custom Movement mode to be used in this zone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	uint8 CustomZonePhysics;

public: //TODO

	/** Build modules this zone will use. */
	UPROPERTY(Instanced, EditAnywhere, BlueprintReadWrite, Category = "Transitions")
	TArray<UAdvKitTransitionBuilderModule*> BuildModules;

public:
	/**
	 * Constructor
	 */
	AAdvKitZone(const FObjectInitializer& ObjectInitializer);

	/**
	 * This flag prevents transition generation if the map has not been loaded but the constructin script 
	 * already runs. In that case prior transitions could be wiped because no neighbour zones are found.
	 */
	bool bDoubleConstructionScriptFix;

	/**
	 * Creates all transitions for this zone. Called either manually or by a transition builder if an 
	 * adjacent zone triggers it.
	 * @param	bZoneMoved	True if this zone was moved, false if it not
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AdvKit")
	void GenerateTransitions(bool bZoneMoved = true);
	virtual void GenerateTransitions_Implementation(bool bZoneMoved = true);

	/**
	 * Creates a new transition point in this zone, specified by the parameters.
	 * @param	WorldSourceLocation		Start of the transition (inside the zone)
	 * @param	WorldTargetLocation 	End of the transition (outside of the zone)
	 * @param	ForCharacter 	Which character class can use the transition
	 * @param	MovementMode 	Movement mode the character will have after the transition
	 * @param	TargetZone		Target zone of the transition or null if used to exit the zone
	 * @param	CustomMovementMode 	Which custom movement mode the character will have after the transition.
	 * @return Created transition component
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual UAdvKitTransitionComponentPoint* RegisterTransition(const FVector& WorldSourceLocation, const FVector& WorldTargetLocation, TSubclassOf<AAdvKitCharacter> ForCharacter, EAdvKitMovementMode MovementMode, class AAdvKitZone* TargetZone = NULL, uint8 CustomMovementMode = 0);

	/**
	 * Creates a new transition area in this zone, specified by the parameters.
	 * @param	MinWorldSourceLocation		Left start of the transition line (inside the zone)
	 * @param	MinWorldTargetLocation 		Left end of the transition line (outside of the zone)
	 * @param	MaxWorldSourceLocation		Right start of the transition (inside the zone)
	 * @param	MaxWorldTargetLocation		Right end of the transition (outside of the zone)
	 * @param	ForCharacter 	Which character class can use the transition
	 * @param	MovementMode 	Movement mode the character will have after the transition
	 * @param	TargetZone		Target zone of the transition or null if used to exit the zone
	 * @param	CustomMovementMode 	Which custom movement mode the character will have after the transition.
	 * @return Created transition component
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual UAdvKitTransitionComponentArea* RegisterTransitionArea(const FVector& MinWorldSourceLocation, const FVector& MinWorldTargetLocation, const FVector& MaxWorldSourceLocation, const FVector& MaxWorldTargetLocation, TSubclassOf<AAdvKitCharacter> ForCharacter, EAdvKitMovementMode MovementMode, class AAdvKitZone* TargetZone = NULL, uint8 CustomMovementMode = 0);

	/**
	 * Add an existing transition to the transitions list
	 * @param	For		Character class that can use the transition
	 * @param	Transition	The transition to add
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual void AddTransition(TSubclassOf<AAdvKitCharacter> For, class UAdvKitTransitionComponent* Transition);

	/**
	 * Removes all transitions that target a specified zone
	 * @param	Other	The zone that transitions target
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual void RemoveExistingTransitionsTo(class AAdvKitZone* Other);

	/**
	 * Checks if a given location is inside the zone.
	 * @param	WorldPosition 	The location to check for
	 * @param	ErrorMargin		How much leeway the check has
	 * @return True if inside zone, false otherwise
	 */
	virtual bool IsPointInside(const FVector& WorldPosition, float ErrorMargin = 0.1f);

	/**
	 * Queries all available transitions for a given character.
	 * @param	ForCharacter 	The character to get transitions for.
	 * @return List of all available transitions.
	 */
	virtual TArray<class UAdvKitTransitionComponent*> GetPossibleTransitions(const class AAdvKitCharacter* ForCharacter);

	/**
	 * Get the next position in this zone in a certain direction. Since zones restrict movement the character can not always
	 * move as it desires, but has to adhere to a direction given by the zone.
	 * @param	FromWorldPosition 	Where the movement starts
	 * @param	WorldDesiredDelta 	The intended direction of the movement
	 * @param	bClamp 	Whether or not to restrict the resulting location to the zone as well
	 * @param	HalfExtent 	Half size of the character, describing its capsule (e.g. capsule radius and half height)
	 * @param	ExtentWorldRotation 	Rotation of the character.
	 * @return The resulting location after the move was restricted to the zone.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AdvKit")
	FVector GetPositionInDirection(const FVector& FromWorldPosition, const FVector& WorldDesiredDelta, bool bClamp, const FVector& HalfExtent, const FRotator& ExtentWorldRotation = FRotator::ZeroRotator);
	virtual FVector GetPositionInDirection_Implementation(const FVector& FromWorldPosition, const FVector& WorldDesiredDelta, bool bClamp, const FVector& HalfExtent, const FRotator& ExtentWorldRotation = FRotator::ZeroRotator);

	/**
	 * Moves a zone location along the zone while keeping it inside the zone.
	 * @param	LocationToMove 	The location object to move
	 * @param	WorldDesiredDelta 	The intended direction of the movement
	 * @param	HalfExtent 	Half size of the character, describing its capsule (e.g. capsule radius and half height)
	 * @param	ExtentWorldRotation 	Rotation of the character.
	 * @return True if location was moved, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool MoveLocationInDirection(UAdvKitZoneLocation* LocationToMove, const FVector& WorldDesiredDelta, const FVector& HalfExtent, const FRotator& ExtentWorldRotation = FRotator::ZeroRotator);

	/**
	 * Gets the zone's forward vector at a specified point.
	 * @param	AtLocation	Location to query the vector at.
	 * @return	Forward vector in world space.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual FVector GetZoneForwardVector(const UAdvKitZoneLocation* AtLocation);
	
	/**
	 * Gets the zone's right vector at a specified point.
	 * @param	AtLocation	Location to query the vector at.
	 * @return	Right vector in world space.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual FVector GetZoneRightVector(const UAdvKitZoneLocation* AtLocation);
	
	/**
	 * Gets the zone's up vector at a specified point.
	 * @param	AtLocation	Location to query the vector at.
	 * @return	Up vector in world space.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual FVector GetZoneUpVector(const UAdvKitZoneLocation* AtLocation);

	/**
	 * Clamps a given location so it is inside the zone.
	 * @param	WorldPosition	Location to clamp.
	 * @param	HalfExtent 	Half size of the character, describing its capsule (e.g. capsule radius and half height)
	 * @param	ExtentWorldRotation 	Rotation of the character.
	 * @return	Location inside zone.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AdvKit")
	FVector ConstrainPositionToZone(const FVector& WorldPosition, const FVector& HalfExtent, const FRotator& ExtentWorldRotation = FRotator::ZeroRotator);
	virtual FVector ConstrainPositionToZone_Implementation(const FVector& WorldPosition, const FVector& HalfExtent, const FRotator& ExtentWorldRotation = FRotator::ZeroRotator);

	/**
	 * Creates a new zone location as close to a given location as possible
	 * @param	WorldPosition	Location the zone location would ideally be at.
	 * @param	HalfExtent 	Half size of the character, describing its capsule (e.g. capsule radius and half height)
	 * @param	ExtentWorldRotation 	Rotation of the character.
	 * @return	Zone Location inside zone.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AdvKit")
	UAdvKitZoneLocation* GetClosestZoneLocation(const FVector& WorldPosition, const FVector& HalfExtent, const FRotator& ExtentWorldRotation = FRotator::ZeroRotator);
	virtual UAdvKitZoneLocation* GetClosestZoneLocation_Implementation(const FVector& WorldPosition, const FVector& HalfExtent, const FRotator& ExtentWorldRotation = FRotator::ZeroRotator);


	/**
	 * Restricts a desired direction to match zone. 
	 * @param	WorldDirection	Direction to restrict.
	 * @param	AtWorldLocation	Location at which the direction should be constrained. (Since zones may be splines a constrained direction can differ based on location).
	 * @return	Direction restricted to zone.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AdvKit")
	FVector ConstrainDirectionToZone(const FVector& WorldDirection, const FVector& AtWorldLocation);
	virtual FVector ConstrainDirectionToZone_Implementation(const FVector& WorldDirection, const FVector& AtWorldLocation);

	/**
	 * Get movement mode of this zone.
	 * @return Movement mode inside the zone.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual EAdvKitMovementMode GetPhysics();

	/**
	 * Get custom movement mode of this zone.
	 * @return Custom movement mode inside the zone.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual uint8 GetCustomPhysics();

	/**
	 * Check if the zone has a specific movement mode.
	 * @param QueryPhysics	The mode to check for.
	 * @return	True if zone has this mode.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool HasPhysics(EAdvKitMovementMode QueryPhysics);

	/**
	 * Check if the zone has a specific custom movement mode.
	 * @param QueryPhysics	The mode to check for.
	 * @return	True if zone has this mode.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool HasCustomPhysics(uint8 QueryPhysics);

	/**
	 * Sets a character class to all transition build modules removing all previous ones
	 * @param CharacterClass	Character class to set
	 */
	void SetCharacterClass(TSubclassOf<AAdvKitCharacter> CharacterClass);


	/**
	 * Adds a character class to all transition build modules if not present, yet
	 * @param CharacterClass	Character class to add
	 */
	void AddCharacterClass(TSubclassOf<AAdvKitCharacter> CharacterClass);

	/**
	 * Removes a character class from all transition build modules
	 * @param CharacterClass	Character class to remove
	 */
	void RemoveCharacterClass(TSubclassOf<AAdvKitCharacter> CharacterClass);

	/**
	 * Query if a character can enter this zone
	 * @param	Character		Character that wants to enter the zone
	 */
	bool IsCharacterSupported(AAdvKitCharacter* Character);

};
