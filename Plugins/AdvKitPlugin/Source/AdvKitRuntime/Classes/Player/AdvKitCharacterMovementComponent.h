// Copyright 2015 Pascal Krabbe

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "AdvKitOrientationComponent.h"
#include "AdvKitTypes.h"
#include "AdvKitCharacterMovementComponent.generated.h"

/** @brief Movement properties of the adventure kit */
USTRUCT(BlueprintType)
struct FAdvKitMovementProperties
{
	GENERATED_USTRUCT_BODY();

	/** Can the character use tight spaces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKitMovementProperties")
	uint32 bCanWalkTightSpace : 1;

	/** Can the character use balancing zones */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKitMovementProperties")
	uint32 bCanWalkBalance: 1;

	/** Can the character climb on ceilings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKitMovementProperties")
	uint32 bCanClimbCeiling : 1;

	/** Can the character climb on walls */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKitMovementProperties")
	uint32 bCanClimbWall : 1;

	/** Can the character climb on ledges */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKitMovementProperties")
	uint32 bCanClimbLedge: 1;

	/** Can the character use ladders */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKitMovementProperties")
	uint32 bCanClimbLadder : 1;


	FAdvKitMovementProperties()
		: bCanWalkTightSpace(true)
		, bCanWalkBalance(true)
		, bCanClimbCeiling(true)
		, bCanClimbWall(true)
		, bCanClimbLedge(true)
		, bCanClimbLadder(true)
	{
	}
};


/**
 * @brief Default movement component for the @see AAdvKitCharacter. This handles zone movement and target locking.
 */
UCLASS(Blueprintable, NotPlaceable)
class ADVKITRUNTIME_API UAdvKitCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	/** Keep track of last acceleration that was not zero in case the character needs to face this direction */
	FVector LastNonZeroAcceleration;

public:
	/**
	 * Constructor
	 */
	UAdvKitCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);


	/**
	 * Hard set the velocity variables
	 * @param	NewVelocity 	New velocity in world space
	 * @param	NewAcceleration 	New acceleration in world space
	 */
	void SetMovementVariables(FVector NewVelocity, FVector NewAcceleration);

	/**
	 * Set all movement variables to zero, halting movement
	 */
	void ZeroMovementVariables();

	/** Movement properties for the adventure kit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Capabilities", meta = (DisplayName = "Adv Movement Capabilities", Keywords = "Nav Agent"))
	FAdvKitMovementProperties AdvMovementProps;

	/**
	 * Get the advkit movement properties
	 */
	FORCEINLINE const FAdvKitMovementProperties* GetAdvMovementProperties() const { return &AdvMovementProps; }

	/**
	* Get the advkit movement properties
	*/
	FORCEINLINE FAdvKitMovementProperties* GetAdvMovementProperties() { return &AdvMovementProps; }

	/**
	 * Set a new movement mode
	 * @param	NewMovementMode	The new movement mode to set
	 * @param	NewCustomMode	A custom movement mode to set
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual void SetAdvMovementMode(EAdvKitMovementMode NewMovementMode, uint8 NewCustomMode = 0);

	/** @return True if the character is currently climbing a wall. */
	virtual bool IsClimbingWall() const;

	/** @return True if the character is currently climbing a ceiling. */
	virtual bool IsClimbingCeiling() const;

	/** @return True if the character is currently climbing a ledge. */
	virtual bool IsClimbingLedge() const;

	/** @return True if the character is currently climbing a ladder. */
	virtual bool IsClimbingLadder() const;

	/** @return True if the character is currently walking in a tight space. */
	virtual bool IsWalkingTightspace() const;

	/** @return True if the character is currently in a balance zone. */
	virtual bool IsWalkingBalance() const;

	/**
	 * Tries to let go of a zone (transition downwards or leaving a wall)
	 * @return	True if a transition happened
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool LetGoOfZone();

	/**
	 * Tries to transition upwards on a zone
	 * @return	True if a transition happened
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool JumpUpZone();

	/**
	 * Attempts to do a transition inside a zone to a given direction
	 * @param	ForZone 	Zone to start transition in 
	 * @param	ForWorldPosition 	Where to start the transition
	 * @param	InWorldDirection 	The direction the player wants to transition in
	 * @param	bIsJump 	True if the character attempts to jump during the transition
	 * @return	True if a transition happened
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool TryToTransition(AAdvKitZone* ForZone, const FVector& ForWorldPosition, const FVector& InWorldDirection = FVector::ZeroVector, bool bIsJump = false);
	
	/**
	 * Queries all possible transitions a zone has for the character
	 * @param	ForZone 	Zone to get transitions from
	 * @param	ForWorldPosition 	Where to start the transition
	 * @param	InWorldDirection 	The direction the player wants to transition in
	 * @param	bIsJump 	True if the character attempts to jump during the transition
	 * @return	List of possible transitions
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual UAdvKitTransitionComponent* GetPossibleTransition(AAdvKitZone* ForZone, const FVector& ForWorldPosition, const FVector& InWorldDirection = FVector::ZeroVector, bool bIsJump = false);
	
	/**
	 * Checks if the character can use a transition in a given zone
	 * @param	ForZone 	Zone to start transition in
	 * @param	ForWorldPosition 	Where to start the transition
	 * @param	InWorldDirection 	The direction the player wants to transition in
	 * @param	bIsJump 	True if the character attempts to jump during the transition
	 * @return	True if the character can transition with the provided arguments
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool CanTransition(AAdvKitZone* ForZone, const FVector& ForWorldPosition, const FVector& InWorldDirection = FVector::ZeroVector, bool bIsJump = false);

	/**
	 * Get the current movement mode
	 * @return	Current movement mode
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual EAdvKitMovementMode GetAdvMovementMode() const;

	/**
	 * Get the current custom movement mode
	 * @return	Current custom movement mode
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	uint8 GetAdvCustomMovementMode() const;
	
	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaSeconds, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	//End UActorComponent Interface

	//Begin UMovementComponent Interface
	virtual float GetMaxSpeed() const override;
	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;
	//End UMovementComponent Interface

	//Begin UCharacterMovementComponent interface
	virtual void StartNewPhysics(float DeltaSeconds, int32 Iterations) override;
	virtual void PhysWalking(float DeltaSeconds, int32 Iterations) override;
	virtual void PhysicsRotation(float DeltaSeconds) override;
	virtual bool DoJump(bool bReplayingMoves) override;
	//End UCharacterMovementComponent interface

protected:

	/**
	 * Takes care of movement when climbing a wall
	 * @param	DeltaSeconds 	Time since last frame
	 * @param	Iterations	Physics iterations for sub stepping
	 */
	virtual void PhysClimbWall(float DeltaSeconds, int32 Iterations);

	/**
	 * Takes care of movement when climbing a ceiling
	 * @param	DeltaSeconds 	Time since last frame
	 * @param	Iterations	Physics iterations for sub stepping
	 */
	virtual void PhysClimbCeiling(float DeltaSeconds, int32 Iterations);

	/**
	* Takes care of movement when climbing a ladder
	* @param	DeltaSeconds 	Time since last frame
	* @param	Iterations	Physics iterations for sub stepping
	*/
	virtual void PhysClimbLadder(float DeltaSeconds, int32 Iterations);

	/**
	* Takes care of movement when climbing a ledge
	* @param	DeltaSeconds 	Time since last frame
	* @param	Iterations	Physics iterations for sub stepping
	*/
	virtual void PhysClimbLedge(float DeltaSeconds, int32 Iterations);

	/**
	* Takes care of movement when inside a tight space
	* @param	DeltaSeconds 	Time since last frame
	* @param	Iterations	Physics iterations for sub stepping
	*/
	virtual void PhysTightSpace(float DeltaSeconds, int32 Iterations);

	/**
	 * Takes care of movement when balancing
	 * @param	DeltaSeconds 	Time since last frame
	 * @param	Iterations	Physics iterations for sub stepping
	 */
	virtual void PhysBalancing(float DeltaSeconds, int32 Iterations);

	/**
	 * Delegates movement to the current action
	 * @param	DeltaSeconds 	Time since last frame
	 * @param	Iterations	Physics iterations for sub stepping
	 */
	virtual void PhysActionDriven(float DeltaSeconds, int32 Iterations);

public:
	/** How far the character can jump horizontally between ladder and ledge */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adv Character Movement: Transitions")
	float JumpDistanceLedgeLadderHorizontal;

	/** How far ahead to trace for a ledge below when walking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Adv Character Movement: Ledge")
	float LedgePrecognitionDistance;

	/** How far down to trace to consider a cliff for ledges*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Adv Character Movement: Ledge")
	float LedgeDownTraceDistance;

	/** How far above the ground to start tracing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Adv Character Movement: Ledge")
	float LedgeSmallHeightOffset;
	
	/** How far the character can jump vertically from a ledge */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Adv Character Movement: Ledge")
	float LedgeJumpDistanceVertical;

	/** How far the character can jump horizontally from a ledge */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Adv Character Movement: Ledge")
	float LedgeJumpDistanceHorizontal;

	/** How fast the character can climb on a ledge */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Adv Character Movement: Ledge")
	float MaxLedgeClimbSpeed;

	/** What local offset to keep from a ledge while climbing it */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adv Character Movement: Ledge")
	FVector LedgeOffset;

	/** How far the character can jump horizontally from a ladder */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Adv Character Movement: Ladder")
	float LadderJumpDistanceHorizontal;

	//How fast the character climbs a ladder
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Adv Character Movement: Ladder")
	float MaxLadderClimbSpeed;

	//What offset to have from the ladder the character is climbing on
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Adv Character Movement: Ladder")
	FVector LadderOffset;

	//What offset to keep from it when climbing a wall
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Adv Character Movement: Wall")
	FVector WallOffset;

	//How fast the character can climb on a wall
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Adv Character Movement: Wall")
	float MaxWallClimbSpeed;

	//How fast the character can climb on a ceiling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adv Character Movement: Ceiling")
	float MaxCeilingClimbSpeed;

	//What offset to keep from the ceiling if the character is climbing on it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adv Character Movement: Ceiling")
	FVector CeilingOffset;

	//The minimum orientation difference betweeen the character and a wall to start transitioning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adv Character Movement: Ceiling")
	float CeilingMinTransitionDot;

	//What offset to keep from it when in a tight space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adv Character Movement: Tight Space")
	FVector TightSpaceOffset;

	//How fast the character moves inside a tight space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adv Character Movement: Tight Space")
	float MaxTightSpaceWalkSpeed;

	//How fast the character moves when balancing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adv Character Movement: Balance")
	float MaxBalanceWalkSpeed;

	//What offset to keep from a balance zone
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adv Character Movement: Balance")
	FVector BalanceOffset;
	
	//How fast the character turns inside a balance zone in degrees per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adv Character Movement: Balance")
	float BalanceRotationRate;
	/** The balance the character has when in a balancing zone */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Adv Character Movement: Balance", Transient)
	float CurrentBalance;
	
	/** Scale to apply to player input trying to restore balance */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Adv Character Movement: Balance", Transient)
	float CounterBalanceMultiplier;

	/** If input does not align correctly with balance direction, this is the min dot required for input to count as counter balance */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Adv Character Movement: Balance", Transient)
	float MinCounterBalanceDot;

	/** How much the current forward and input directions need to differ in order for the character to turn around */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Adv Character Movement: Balance", Transient)
	float MaxBalanceTurnAroundDot;

	/** How fast to change the balance to the target balance */
	UPROPERTY(BlueprintReadWrite, Category = "Adv Character Movement: Balance", Transient)
	float BalanceChangePerSecond;

	/** How fast to change the balance to the target balance at minimum */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Adv Character Movement: Balance")
	float MinBalanceChangePerSecond;

	/** How fast to change the balance to the target balance at maximum */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Adv Character Movement: Balance")
	float MaxBalanceChangePerSecond;

	/*Max balance, if Abs(Balance) is bigger or equals to this the character falls off*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adv Character Movement: Balance")
	float MaxBalance;

	/** How fast to change the target balance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adv Character Movement: Balance")
	float BalanceChangeTime;

	/** TImer to change target balance */
	FTimerHandle ChangeBalanceTimerHandle;

	/** Timer callback to change target balance */
	UFUNCTION()
	virtual void ChangeTargetBalance();

public:
	/**
	 * Query the current zone of the character
	 * @return	Zone the character is currently in
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual class AAdvKitZone* GetZone();

	/**
	 * Get the current zone location of the character
	 * @return	Current zone location
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual UAdvKitZoneLocation* GetZoneLocation();

	/**
	 * Moves the character's zone location object with the given parameters
	 * @param	GlobalAcceleration 	Acceleration to use for the intended moving direction
	 * @param	DeltaSeconds 		Delta time since last tick
	 * @param	HalfExtent			Size of the character in the zone (e.g. FVector(Radius,Radius,HalfHeight))
	 * @param	bAutoTransition 	True to try to transition if the character cannot move
	 * @param	ConstrainAcceleration	True to restrict the acceleration vector to the zone
	 * @return	True if location was moved
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool MoveLocationInZone(FVector GlobalAcceleration, float DeltaSeconds, const FVector& HalfExtent, bool bAutoTransition = true, bool ConstrainAcceleration = true);


	DEPRECATED(4.10, "Use GetMaxSpeed() instead")
	virtual FVector GetAccelerationInZone();

	/**
	 * Moves the character inside the current zone with the given parameters
	 * @param	GlobalAcceleration 	Acceleration to use for the intended moving direction
	 * @param	DeltaSeconds 		Delta time since last tick
	 * @param	HalfExtent			Size of the character in the zone (e.g. FVector(Radius,Radius,HalfHeight))
	 * @param	bAutoTransition 	True to try to transition if the character cannot move
	 * @param	ConstrainAcceleration	True to restrict the acceleration vector to the zone
	 * @return	True if location was moved
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool PhysMoveInZone(FVector GlobalAcceleration, FVector WorldOffset, float DeltaSeconds, const FVector& HalfExtent, bool bAutoTransition = true, bool ConstrainAcceleration = true);

	/**
	 * Query what rotation the character should have at a specified location inside the current zone.
	 * @param	Zone 			Zone to get the desired rotation
	 * @param	AtLocation 	Location inside the current zone
	 * @return	Desired rotation of the character
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual FRotator GetDesiredRotationInZone(AAdvKitZone* Zone, UAdvKitZoneLocation* AtLocation);

	/**
	 * Query how far the character should be away from the location inside the zone
	 * @param	Zone 			Zone to get the desired offset from
	 * @param	AtLocation 	Location inside the current zone
	 * @return	Desired offset from the location inside the zone
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual FVector GetDesiredOffsetFromZone(AAdvKitZone* Zone, UAdvKitZoneLocation* AtLocation);

	/**
	 * Query how "big" the character for a specific movement mode in local space.
	 * @param	Zone		Zone to get extent for
	 * @return Half extent of the character e.g (Radius, Radius, Capsule Half Height)
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual FVector GetHalfExtentForZone(AAdvKitZone* Zone);

	/**
	 * Get the location inside the current zone in world space.
	 * @return	World space location inside the current zone
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual FVector GetGlobalLocationInZone();

	/**
	 * Adds root motion to the owning character.
	 * @param	MovementDelta	Translation to be added to the character.
	 * @param	NewRotation		New rotation the character should have.
	 */
	UFUNCTION(BlueprintCallable, Category="AdvKit")
	void AccumulateRootMotion(FVector MovementDelta, FRotator NewRotation);


	/**
	 * Adds root motion rotation to the owning character.
	 * @param	NewRotation		New rotation the character should have.
	 */
	void AccumulateRootMotionRotation(FRotator NewRotation);

	/**
	 * Adds root motion to the owning character.
	 * @param	GlobalDelta	Translation to be added to the character.
	 */
	void AddRootMotionMovement(FVector GlobalDelta);

	/**
	 * Adds root motion rotation to the owning character.
	 * @param	AdditionalRotation	Rotation do be added to the character.
	 */
	void AddRootMotionRotation(FRotator AdditionalRotation);

protected:

	/** AdvKitCharacter movement component belongs to */
	UPROPERTY()
	class AAdvKitCharacter* AdvKitCharacterOwner;

};
