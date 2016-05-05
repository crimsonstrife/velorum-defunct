// Copyright 2015 Pascal Krabbe

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "Actions/AdvKitCharacterAction.h"
#include "AdvKitCharacter.generated.h"

class AAdvKitTargetPoint;

/**
 * @brief Utility struct to allow configuring an inventory item on the character.
 */
USTRUCT()
struct FAdvKitInventoryConfig
{
	GENERATED_USTRUCT_BODY();

	/** Class of the item to create */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AdvKit)
	TSubclassOf<class AAdvKitInventoryItem> InventoryClass;

	/** Whether or not this item will be equipped on start */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AdvKit)
	bool bEquip;
};

/** @brief Helper struct to replicated @see UAnimMontage on the character */
USTRUCT()
struct FRepAdvKitAnimMontage
{
	GENERATED_USTRUCT_BODY();

	/** Anim montage that is being replicated */
	UPROPERTY()
	UAnimMontage* Montage;

	/** Counter to make sure each new play of a montage is replicated */
	UPROPERTY()
	uint32 AnimCounter;

	/** How fast to play the montage */
	UPROPERTY()
	float PlayRate;

	/** Which section to start playing */
	UPROPERTY()
	FName StartSectionName;

	/**
	 * Set the values of the struct
	 * @param	NewMontage 	Montage to play
	 * @param	NewPlayRate 	How fast to play the montage
	 * @param	NewStartSectionName 	Section to start playing at
	 * @return 
	 */
	void SetNewMontage(UAnimMontage* NewMontage, float NewPlayRate = 1.0f, FName NewStartSectionName = NAME_None)
	{
		AnimCounter++;
		Montage = NewMontage;
		PlayRate = NewPlayRate;
		StartSectionName = NewStartSectionName;
	}
};

/**
 * @brief Enumeration to distinguish the integrated camera modes of the kit.
 */
UENUM(BlueprintType)
enum class EAdvKitCameraType : uint8
{
	FirstPerson,
	ThirdPerson,
	Shoulder,
	TargetLocked,
	Unkown
};

/**
 * @brief Base character class of the Adventure Kit. 
 * 
 * This character can use zones, inventory items and execute character actions.
 * It also provides functionality for several different camera styles and locking onto a target.
 */
UCLASS(Blueprintable, abstract)
class ADVKITRUNTIME_API AAdvKitCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	/** Whether or not to automatically enter a zone when the character hits it */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zones")
	bool bEnterZonesFromHitAutomatically;

	/** Whether or not to automatically enter a zone when the character overlaps it */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zones")
	bool bEnterZonesFromOverlapAutomatically;

	/** List of collision channels to ignore while moving inside a zone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zones")
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannelsToIgnoreInZones;

	/** Name of the socket which represents the character's head. Used for first person camera and weapon targeting. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit")
	FName HeadSocket;

	/** Restrict the camera pitch to the head socket's orientation in first person view */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit | First Person")
	bool bFirstPersonClampCameraPitch;

	/** Minimum angle to clamp pitch to in first person */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit | First Person", meta = (EditCondition = bFirstPersonClampCameraPitch))
	float FirstPersonMinCameraPitch;

	/** Maximum angle to clamp pitch to in first person */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit | First Person", meta = (EditCondition = bFirstPersonClampCameraPitch))
	float FirstPersonMaxCameraPitch;

	/** Restrict the camera yaw to the head socket's orientation in first person view*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit | First Person")
	bool bFirstPersonClampCameraYaw;

	/** Minimum angle to clamp yaw to in first person */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit | First Person", meta = (EditCondition = bFirstPersonClampCameraYaw))
	float FirstPersonMinCameraYaw;

	/** Maximum angle to clamp yaw to in first person */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit | First Person", meta = (EditCondition = bFirstPersonClampCameraYaw))
	float FirstPersonMaxCameraYaw;

	/** Restrict the camera roll to the head socket's orientation in first person view*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit | First Person")
	bool bFirstPersonClampCameraRoll;

	/** Minimum angle to clamp roll to in first person */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit | First Person", meta = (EditCondition = bFirstPersonClampCameraRoll))
	float FirstPersonMinCameraRoll;

	/** Maximum angle to clamp roll to in first person */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit | First Person", meta = (EditCondition = bFirstPersonClampCameraRoll))
	float FirstPersonMaxCameraRoll;

	/** If true, this Pawn's pitch will be updated to match the Controller's ControlRotation pitch, if controlled by a PlayerController. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit | First Person")
	bool bUseControllerRotationPitchInFirstPerson;

	/** If true, this Pawn's yaw will be updated to match the Controller's ControlRotation yaw, if controlled by a PlayerController. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit | First Person")
	bool bUseControllerRotationYawInFirstPerson;

	/** If true, this Pawn's roll will be updated to match the Controller's ControlRotation roll, if controlled by a PlayerController. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit | First Person")
	bool bUseControllerRotationRollInFirstPerson;

	/** If true the character takes no damage if it is dodging */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit | Dodge")
	bool bInvincibleDuringDodge;

public:
	/**
	 * Constructor
	 */
	AAdvKitCharacter(const FObjectInitializer& ObjectInitializer);

	/** Follow camera. Tag camera component with "Third Person" auto assign this property.  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AdvKit|Camera")
	class UCameraComponent* ThirdPersonCamera;

	/** First person camera. Tag camera component with "First Person" auto assign this property.  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AdvKit|Camera")
	class UCameraComponent* FirstPersonCamera;

	/** Shoulder camera. Tag camera component with "Shoulder" auto assign this property.  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AdvKit|Camera")
	class UCameraComponent* ShoulderCamera;

	/** Camera used when in target lock mode. Tag camera component with "Target Lock" auto assign this property. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AdvKit|Camera")
	class UCameraComponent* TargetLockCamera;

	/** Health points of the character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvKit, Replicated)
	float Health;

	/** Max health of the character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvKit, Replicated)
	float MaxHealth;


//Actions
	/** Configurable array of actions to instantiate for the character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AdvKit|Actions")
	TArray< TSubclassOf<class UAdvKitCharacterAction> > DefaultActions;

	/** Class of action manager to be used by this character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AdvKit|Actions")
	TSubclassOf<class AAdvKitCharacterActionManager> ActionManagerClass;

	/** Action manager used by this character */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AdvKit|Actions", Replicated)
	class AAdvKitCharacterActionManager* ActionManager;

public:
	/**
	 * Get the action that the character is currently executing.
	 * @return	Action that is currently executed
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit|Actions")
	UAdvKitCharacterAction* GetActiveAction() const;

//Inventory
	/** How far away items and switches can be from the camera to be found with clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AdvKit")
	float PointAndClickTraceDistance;

	/** Configuration of items to create for the character by default */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray< FAdvKitInventoryConfig > DefaultInventory;

	/** Class of the inventory manager for this character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<class AAdvKitInventoryManager> InventoryManagerClass;

	/** Inventory manager this character uses */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory", Replicated)
	class AAdvKitInventoryManager* InventoryManager;

	/**
	 * Equip the next item in the character's inventory
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void NextItem();

	/**
	 * Equip the previous item in the character's inventory
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void PreviousItem();

	/**
	 * Get Aim direction offset from character rotation
	 * @return	Aim offset rotation
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual FRotator GetAimOffsets() const;


	/**
	 * Get the rotation for a weapon to fire at. X-Axis of the rotator is forward for the shot
	 * @param	Weapon 	Weapon that wants to fire
	 * @param	FireLocation 	Location from which the weapon wants to fire
	 * @return Rotation of the aim
	 */
	virtual FRotator GetAdjustedAimFor(class AAdvKitWeapon* Weapon, FVector FireLocation);
	
	/**
	 * Get the starting location for tracing the target, e.g. the player's eyes
	 * @param	Weapon 	Weapon that wants to trace
	 * @return	Starting location for the trace
	 */
	virtual FVector GetWeaponStartTraceLocation(class AAdvKitWeapon* Weapon = NULL);


//Targeting
	/** How far away targets can be for auto targeting */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target")
	float MaxTargetDistance;

	/** Currently targeted component */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Target")
	AAdvKitTargetPoint* CurrentTarget;

	/** Whether the target is locked in */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Target")
	bool bLockTarget;

	/** Whether or not to face a target that has been locked */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	bool bFaceLockedTarget;

	/**
	 * Get the maximum distance at which targets should be considered for the character
	 * @return	Distance at which targets are considered.
	 */
	virtual float GetMaximumTargetDistance();

	/**
	 * Gets all targets the player can reach and lock on to.
	 * @return	Reachable targets
	 */
	UFUNCTION(BlueprintCallable, Category = "Target")
	virtual TArray<AAdvKitTargetPoint*> GetAllAvailableTargets();

	/**
	 * Filter a list of targets and get the best (e.g. closest) one
	 * @param	AvailableTargets	List to filter for target
	 * @return	Chosen target
	 */
	virtual AAdvKitTargetPoint* GetBestTarget(const TArray<AAdvKitTargetPoint*> &AvailableTargets);
	
	/**
	 * Set a new target for the character
	 * @param	NewTarget	The target to set
	 * @param	bLock	True to lock onto the target
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual void SetTarget(AAdvKitTargetPoint* NewTarget, bool bLock = false);

//Movement / Input

	/**
	 * Get the character's adv kit movement properties
	 * @return	The character's adventure kit movement properties
	 */
	virtual const struct FAdvKitMovementProperties* GetAdvMovementProperties() const;

	/**
	 * Get the character's adv kit movement component.
	 * @return The character's adv kit movement component.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual class UAdvKitCharacterMovementComponent* GetAdvMovementComponent() const;

	/**
	 * Wraps AddMovementInput and transforms local direction in case of certain zone movement modes (Wall, Ledge and Ladder) instead of using world direction.
	 *
	 * @param	WorldDirection	Direction in world space to apply input
	 * @param	LocalDirection 	Direction of input in character space
	 * @param	ScaleValue		Scale to apply to input. This can be used for analog input, ie a value of 0.5 applies half the normal value, while -1.0 would reverse the direction.
	 * @param	bForce			If true always add the input, ignoring the result of IsMoveInputIgnored().
	 * @see AddMovementInput
	 */
	UFUNCTION(BlueprintCallable, Category = "Character|Input", meta = (Keywords = "AddInput"))
	virtual void AddConditionalMovementInput(FVector WorldDirection, FVector LocalDirection, float ScaleValue = 1.0f, bool bForce = false);


	/**
	 * Use a usable with a given item (e.g. a lock with a key)
	 * @param	Useable 	Usable to use with optional item
	 * @param	WithItem 	Optional item to use usable with
	 */
	UFUNCTION()
	void Use(class AAdvKitUsable* Usable = NULL, class AAdvKitInventoryItem* WithItem = nullptr);

	/**
	 * Server callback to use a usable with a given item (e.g. a lock with a key)
	 * @param	Useable 	Usable to use with optional item
	 * @param	WithItem 	Optional item to use usable with
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUse(class AAdvKitUsable* Useable = NULL, class AAdvKitInventoryItem* WithItem = NULL);
	virtual void ServerUse_Implementation(class AAdvKitUsable* Useable = NULL, class AAdvKitInventoryItem* WithItem = NULL);
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/** Tries to start a new character action on this character
	 *
	 * @param	ActionClass	Class of the action to start
	 * @param	Arguments	Arguments to supply when starting the action.
	 * @return	True if the given action was started
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool StartAction(TSubclassOf<class UAdvKitCharacterAction> ActionClass, class UAdvKitCharacterAction_Arguments* Arguments = NULL);

	/** Tries to start a transition action to a given movement mode
	 *
	 * @param	NewMode	Target movement mode of the transition
	 * @param	Arguments	Arguments to supply when starting the action.
	 * @return	True if a transition was started
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool StartTransitionActionTo(EAdvKitMovementMode NewMode, class UAdvKitCharacterAction_Arguments* Arguments = NULL);

	/**
	 * Reloads the character's current weapon
	 */
	UFUNCTION(BlueprintCallable, Category = AdvKit)
	virtual void Reload();
	
	/**
	 * Starts firing with the currently equipped weapon
	 * @param	FireMode	Index of fire mode to start firing 
	 */
	UFUNCTION(BlueprintCallable, Category = AdvKit)
	virtual void StartFire(uint8 FireMode = 0);

	/**
	 * Stops firing the currently equipped weapon
	 */
	UFUNCTION(BlueprintCallable, Category = AdvKit)
	virtual void StopFire();

	/**
	 * Restricts the controller's view rotation. By default only used in first person to
	 * restrict the camera if configured.
	 */
	virtual void RestrictViewRotation(FRotator& DesiredLocalRotation);

public:

	/**
	 * Make the character dodge 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AdvKit")
	void Dodge();
	virtual void Dodge_Implementation();

	/**
	 * Server callback to make the character dodge
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void DodgeServer();
	virtual void DodgeServer_Implementation();

	/**
	 * Check if the character is dodging
	 * @return	True if dodging
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AdvKit")
	bool IsDodging() const;
	virtual bool IsDodging_Implementation() const;

	/**
	 * If the character has multiple cameras, switch to the next one
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual void NextCamera();

	/**
	 * Set camera style to first person
	 * @param	True to enable first person, false to disable it
	 */
	virtual void SetFirstPersonEnabled(bool bEnabled);
	
	/**
	 * Server callback to set camera style to first person
	 * @param	True to enable first person, false to disable it
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void SetFirstPersonEnabledServer(bool bEnabled);
	virtual void SetFirstPersonEnabledServer_Implementation(bool bEnabled);

	/**
	 * Set a specific camera mode.
	 * @param	Camera mode to set
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual void SetCameraMode(EAdvKitCameraType NewMode);

	/**
	 * Server callback to set a specific camera mode.
	 * @param	Camera mode to set
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void SetCameraModeServer(EAdvKitCameraType NewMode);
	virtual void SetCameraModeServer_Implementation(EAdvKitCameraType NewMode);

	/**
	 * Set a specific camera mode by the camera component
	 * @param	Camera component to set mode from
	 */
	virtual void SetCameraModeByComponent(UCameraComponent* NewCameraComponent);

	/**
	 * Set a specific camera mode to be the active camera
	 * @param	Camera component to activate
	 * @param	True if activation was successful
	 */
	virtual bool SetActiveCameraComponent(UCameraComponent* NewCameraComponent);

	/**
	 * Get the current camera mode
	 * @return	Current camera mode
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual EAdvKitCameraType GetCameraMode();
	
	/** Active camera mode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	EAdvKitCameraType CameraType;

	/** How fast to switch between cameras */
	float CameraLerpSpeed;
	/** Current interpolation alpha of camera switch */
	float CameraLerpAlpha;
	/** Exponent of interpolation camera curve */
	float CameraLerpExponent;

	/** Previous camera component */
	UCameraComponent* LastCameraComponent;

	/** Currently active camera component */
	UCameraComponent* CurrentCameraComponent;

	/**
	 * Enter the zone closest to the character
	 * @return	True if zone was entered
	 */
	virtual bool EnterClosestZone();

protected:
	/** Current location in the zone the character currently is in */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "AdvKit", Replicated)
	class UAdvKitZoneLocation* LocationInZone;

public:
	/**
	 * Set a new zone for the character
	 * @param	NewZone		The new zone to set
	 * @param	bSnap		Snap character to the new zone
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual void SetZone(class AAdvKitZone* NewZone, bool bSnap = true);

	/**
	 * Server callback to a new zone for the character
	 * @param	NewZone	The new zone to set
	 * @param	bSnap		Snap character to the new zone
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void SetZoneServer(AAdvKitZone* NewZone, bool bSnap = true);
	virtual void SetZoneServer_Implementation(AAdvKitZone* NewZone, bool bSnap = true);


	/**
	 * Check if character can enter a zone
	 * @param	Zone 	Zone to enter
	 * @return	True if zone can be entered
	 */
	UFUNCTION(BlueprintCallable, Category="Zones")
	virtual bool CanEnterZone(AAdvKitZone* Zone);

	/**
	 * Attempts to enter a given zone
	 * @param	Zone 	Zone to enter
	 * @return	True if zone was entered
	 */
	UFUNCTION(BlueprintCallable, Category="Zones")
	virtual bool EnterZone(AAdvKitZone* Zone);

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void EnterZoneServer(AAdvKitZone* NewZone);
	virtual void EnterZoneServer_Implementation(AAdvKitZone* NewZone);

	/**
	 * Gets the zone the character is currently in.
	 * @return	Current zone
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual class AAdvKitZone* GetZone() const;

	/**
	 * Set a new zone and zone location for the character
	 * @param	NewLocation		New zone location to set
	 * @param	bSnap		Snap character to the new zone
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual void SetZoneLocation(UAdvKitZoneLocation* NewLocation, bool bSnap = true);


	/**
	 * Server callback to set a new zone and zone location for the character
	 * @param	NewLocation		New zone location to set
	 * @param	bSnap		Snap character to the new zone
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void SetZoneLocationServer(UAdvKitZoneLocation* NewLocation, bool bSnap = true);
	virtual void SetZoneLocationServer_Implementation(UAdvKitZoneLocation* NewLocation, bool bSnap = true);

	/**
	 * Get the current zone location of the character
	 * @return	Current zone location
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual UAdvKitZoneLocation* GetZoneLocation();


	/**
	 * Use the usable closest to the character
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool UseClosestUseable();

	/**
	 * Enable the mouse cursor for point and click
	 * @param	bEnabled 	True to enable, false to disable
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	void SetMouseCursorEnabled(bool bEnabled = true);

	/**
	 * Is the character currently in first person mode?
	 * @return True if in first person mode
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool IsFirstPerson() const;

	/**
	 * Is the character currently in third person mode?
	 * @return True if in third person mode
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool IsThirdPerson() const;

	/**
	 * Is the character currently in shoulder camera mode?
	 * @return True if in shoulder camera mode
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool IsShoulderCam() const;

	/**
	 * Hook method to block incoming damage.
	 * @param	Damage	How much damage to apply
	 * @param	DamageEvent		Data package that fully describes the damage received
	 * @param	EventInstigator	The Controller responsible for the damage
	 * @param	DamageCauser	The Actor that directly caused the damage(e.g.the projectile that exploded, the rock that landed on you)
	 * @return	The amount of damage remaining after it was blocked
	 */
	UFUNCTION(Category = "AdvKit")
	float BlockDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);
	
	/**
	 * Hook method to block any incoming damage.
	 * @param	DamageReceived	How much damage to apply
	 * @param	DamageType		Type of damage
	 * @param	InstigatedBy	The Controller responsible for the damage
	 * @param	DamageCauser	The Actor that directly caused the damage(e.g.the projectile that exploded, the rock that landed on you)
	 * @return	The amount of damage remaining after it was blocked
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "AdvKit")
	float BlockAnyDamage(float DamageReceived, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	virtual float BlockAnyDamage_Implementation(float DamageReceived, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	/**
	 * Hook method to block any incoming radial damage.
	 * @param	DamageReceived	How much damage to apply
	 * @param	DamageType		Type of damage
	 * @param	Origin			Origin of the radial damage
	 * @param	HitInfo			Where the damage actually hit the character
	 * @param	InstigatedBy	The Controller responsible for the damage
	 * @param	DamageCauser	The Actor that directly caused the damage(e.g.the projectile that exploded, the rock that landed on you)
	 * @return	The amount of damage remaining after it was blocked
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "AdvKit")
	float BlockRadialDamage(float DamageReceived, const class UDamageType* DamageType, FVector Origin, const struct FHitResult& HitInfo, class AController* InstigatedBy, AActor* DamageCauser);
	virtual float BlockRadialDamage_Implementation(float DamageReceived, const class UDamageType* DamageType, FVector Origin, const struct FHitResult& HitInfo, class AController* InstigatedBy, AActor* DamageCauser);

	/**
	 * Hook method to block any incoming point damage.
	 * @param	DamageReceived	How much damage to apply
	 * @param	DamageType		Type of damage
	 * @param	HitLocation		Where the damage hit the character
	 * @param	HitNormal		Normal of the hit on the character 
	 * @param	HitComponent	Which component was hit
	 * @param	BoneName		Which bone in the component was hit
	 * @param	ShotDirection	Where the damage came from
	 * @param	InstigatedBy	The Controller responsible for the damage
	 * @param	DamageCauser	The Actor that directly caused the damage(e.g.the projectile that exploded, the rock that landed on you)
	 * @return	The amount of damage remaining after it was blocked
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "AdvKit")
	float BlockPointDamage(float DamageReceived, const class UDamageType* DamageType, FVector HitLocation, FVector HitNormal, class UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, class AController* InstigatedBy, AActor* DamageCauser);
	virtual float BlockPointDamage_Implementation(float DamageReceived, const class UDamageType* DamageType, FVector HitLocation, FVector HitNormal, class UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, class AController* InstigatedBy, AActor* DamageCauser);

	/**
	 * Check if the character is still alive
	 * @return	True if still alive
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AdvKit")
	bool IsAlive();
	virtual bool IsAlive_Implementation();

	/**
	 * Called when the character died
	 * @param	Killer 		Who killed the character
	 * @param	DamageEvent 	The damage event responsible for the kill
	 * @param	HitLocation 	Where the damage that killed came from
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "AdvKit")
	void Died(class AController* Killer, struct FDamageEvent const& DamageEvent, FVector HitLocation);
	virtual void Died_Implementation(class AController* Killer, struct FDamageEvent const& DamageEvent, FVector HitLocation);


	/**
	 * Callback when anim montage was replicated.
	 */
	UFUNCTION()
	virtual void OnRep_RepAnimMontage();
	

	/**
	 * Server callback to play an AnimMontage.
	 * @param	AnimMontage 	Montage to play
	 * @param	InPlayRate	Speed at which to play montage
	 * @param	StartSectionName 	Where to start playing
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void PlayAnimMontageServer(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);
	virtual void PlayAnimMontageServer_Implementation(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	/** Property that contains all required values for replicated the played AnimMontage*/
	UPROPERTY(ReplicatedUsing = OnRep_RepAnimMontage)
	FRepAdvKitAnimMontage RepAnimMontage;


public:
	//Begin AActor interface
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyActorBeginOverlap(class AActor* OtherActor) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	//End AActor interface

	//Begin APawn interface
	virtual FVector GetPawnViewLocation() const override;
	virtual FRotator GetViewRotation() const override;
	virtual FRotator GetBaseAimRotation() const override;
	virtual bool ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const override;
	//End APawn interface

	//Begin ACharacter Interface
	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;
	virtual void StopAnimMontage(class UAnimMontage* AnimMontage = NULL) override;
	virtual void Jump() override;
	virtual void Crouch(bool bClientSimulation = false);
	virtual void FaceRotation(FRotator NewControlRotation, float DeltaSeconds) override;
	virtual bool CanJumpInternal_Implementation() const override;
	virtual void SetBase(UPrimitiveComponent* NewBase, const FName BoneName = NAME_None, bool bNotifyActor = true) override;
	//End ACharacter Interface
};

