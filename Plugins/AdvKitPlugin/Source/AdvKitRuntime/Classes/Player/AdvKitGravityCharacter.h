// Copyright 2015 Pascal Krabbe

#pragma once

#include "AdvKitCharacter.h"
#include "AdvKitGravityCharacter.generated.h"

class UAdvKitGravityCharacterMovementComponent;

/**
 * @brief Character class that supports arbitrary gravity provided by an orientation component.
 */
UCLASS(Blueprintable, abstract)
class ADVKITRUNTIME_API AAdvKitGravityCharacter : public AAdvKitCharacter
{
	GENERATED_BODY()

public:
	/** Orientation component that provides gravity methods */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AdvKit", Replicated)
	class UAdvKitOrientationComponent* WorldOrientationComponent;

	/** Component that traces down when orientation should be updated by trace */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AdvKit")
	class UAdvKitTraceUtilityComponent* TraceWorldOrientation;


public:
	/**
	 * Constructor 
	 */
	AAdvKitGravityCharacter(const FObjectInitializer& ObjectInitializer);

	/**
	 * Checks if a hit result is a valid floor for wall walking
	 * @param	NewFloor 	The floor to test
	 * @return	True if it is valid for wall walking
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "AdvKit")
	bool CanWallWalkOn(FFindFloorResult NewFloor);
	virtual bool CanWallWalkOn_Implementation(FFindFloorResult NewFloor);

	/**
	 * Set a new gravity orientation on the orientation component
	 * @param	GravityUpVector 	New Up vector for the gravity
	 * @param	Origin	Origin of the gravitational pull
	 * @param	bForce 	Hard set new gravity instead of interpolating
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual void SetNewOrientation(FVector GravityUpVector, FVector Origin = FVector::ZeroVector, bool bForce = false);

	/**
	 * Server callback to set a new gravity orientation on the orientation component
	 * @param	GravityUpVector 	New Up vector for the gravity
	 * @param	Origin	Origin of the gravitational pull
	 * @param	bForce 	Hard set new gravity instead of interpolating
	 */
	UFUNCTION(Server, Unreliable, WithValidation)
	void SetNewOrientationServer(FVector GravityUpVector, FVector Origin = FVector::ZeroVector, bool bForce = false);
	virtual void SetNewOrientationServer_Implementation(FVector GravityUpVector, FVector Origin = FVector::ZeroVector, bool bForce = false);

	/**
	 * Get the gravity movement component of the character.
	 * @return	The gravity movement component of the character.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	UAdvKitGravityCharacterMovementComponent* GetAdvGravityMovementComponent() const;

protected:

	//Begin ACharacter Interface
	virtual void FaceRotation(FRotator NewControlRotation, float DeltaSeconds) override;
	//End ACharacter Interface

public:
	// Begin AActor Interface
	virtual void PostInitializeComponents() override;
	// End AActor Interface
};

