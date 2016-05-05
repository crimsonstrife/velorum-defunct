// Copyright 2015 Pascal Krabbe

#pragma once

#include "GameFramework/Actor.h"
#include "AdvKitProjectile.generated.h"

/**
 * @brief Simple projectile class with a sphere root and projectile movement component with
 * callbacks for projectile death.
 */
UCLASS(abstract, Meta = (ChildCanTick))
class ADVKITRUNTIME_API AAdvKitProjectile : public AActor
{
	GENERATED_BODY()
protected:
	/** Destroy the particle if movement stops */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	bool bDestroyOnStop;

	/** Name of the RootComponent. Use this name if you want to use a different class (with ObjectInitializer.SetDefaultSubobjectClass). */
	static FName RootComponentName;

	/** Root component for collision handling */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AdvKit")
	class USphereComponent* DefaultRootComponent;

	/** Name of the MovementComponent. Use this name if you want to use a different class (with ObjectInitializer.SetDefaultSubobjectClass). */
	static FName MovementComponentName;

	/** Movement component for the projectile */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AdvKit")
	class UProjectileMovementComponent* MovementComponent;

public:
	/**
	 * Constructor
	 */
	AAdvKitProjectile(const FObjectInitializer& ObjectInitializer);

	//Begin AActor Interface
	virtual void PostInitializeComponents() override;
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;
	//End AActor Interface

	/**
	 * Callback when projectile hit something.
	 * @param	Impact 	Hit that triggered the event
	 */
	UFUNCTION(BlueprintNativeEvent, Category="AdvKit")
	void OnImpact(const FHitResult& Impact);
	virtual void OnImpact_Implementation(const FHitResult& Impact);

	/**
	 * Initializes the projectile movement into a specified direction.
	 * @param	Direction	Direction the projectile should fly in
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual void Init(const FVector& Direction);
};
