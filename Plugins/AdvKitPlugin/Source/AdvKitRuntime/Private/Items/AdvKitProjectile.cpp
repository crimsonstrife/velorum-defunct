// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Items/AdvKitProjectile.h"

FName AAdvKitProjectile::RootComponentName(TEXT("DefaultRoot"));
FName AAdvKitProjectile::MovementComponentName(TEXT("Movement"));

AAdvKitProjectile::AAdvKitProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultRootComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, RootComponentName);
	RootComponent = DefaultRootComponent;

	MovementComponent = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, MovementComponentName);
	MovementComponent->SetUpdatedComponent(DefaultRootComponent);

	bNetLoadOnClient = true;
	bDestroyOnStop = true;
	bReplicateMovement = true;
}

void AAdvKitProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (bDestroyOnStop)
	{
		MovementComponent->OnProjectileStop.AddDynamic(this, &AAdvKitProjectile::OnImpact);
	}
}

void AAdvKitProjectile::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	if (MovementComponent)
	{
		MovementComponent->Velocity = NewVelocity;
	}
}

void AAdvKitProjectile::Init(const FVector& Direction)
{
	MovementComponent->Velocity = Direction.GetSafeNormal() * MovementComponent->InitialSpeed;
}

void AAdvKitProjectile::OnImpact_Implementation(const FHitResult& Impact)
{
	if (Role == ROLE_Authority)
	{
		Destroy();
	}
}
