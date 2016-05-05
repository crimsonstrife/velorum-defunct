// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"

#include "Player/AdvKitCharacterMovementComponent.h"
#include "Player/AdvKitCharacter.h"
#include "Player/AdvKitPlayerController.h"
#include "Player/AdvKitInventoryManager.h"
#include "Player/AdvKitCharacterActionManager.h"
#include "Player/AdvKitTraceUtilityComponent.h"
#include "Player/AdvKitTargetPoint.h"

#include "Actions/Movement/AdvKitCA_ZoneTransition.h"
#include "Actions/Movement/AdvKitCA_Dodge.h"

#include "Environment/AdvKitTransitionComponent.h"

#include "Items/AdvKitUsable.h"
#include "Items/AdvKitInventoryItem.h"
#include "Items/AdvKitWeapon.h"
#include "Items/AdvKitWeapon.h"

#include "Environment/AdvKitZone.h"
#include "Environment/AdvKitZoneLocation.h"

#include "Net/UnrealNetwork.h"

AAdvKitCharacter::AAdvKitCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UAdvKitCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(30.f, 96.0f);

	// Set turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	bUseControllerRotationYawInFirstPerson = true;
	CameraType = EAdvKitCameraType::ThirdPerson;

	// Configure character movement
    auto MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->bOrientRotationToMovement = true; // Character moves in the direction of input...
		MoveComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
		MoveComp->JumpZVelocity = 600.f;
		MoveComp->AirControl = 0.2f;
		MoveComp->Buoyancy = 1;
	}

	CollisionChannelsToIgnoreInZones.Add(ECollisionChannel::ECC_GameTraceChannel1);
	bEnterZonesFromHitAutomatically = true;
	bEnterZonesFromOverlapAutomatically = true;


	//Default manager classes
	InventoryManagerClass = AAdvKitInventoryManager::StaticClass();
	ActionManagerClass = AAdvKitCharacterActionManager::StaticClass();

	PointAndClickTraceDistance = 1000.0f;
	MaxTargetDistance = 500.0f;

	//Health
	Health = 100;
	MaxHealth = 100;

	bInvincibleDuringDodge = true;

	//Multiplayer
	bReplicates = true;

	//Camera stuff

	//Active camera component is handled by character, no need for auto finding
	bFindCameraComponentWhenViewTarget = false;

	CameraLerpAlpha = 1.0f;
	CameraLerpSpeed = 1.0f;
	CameraLerpExponent = 2.0f;

	bFirstPersonClampCameraPitch = true;
	bFirstPersonClampCameraYaw = true;
	bFirstPersonClampCameraRoll = true;

	FirstPersonMinCameraPitch = -89;
	FirstPersonMaxCameraPitch = 89;
	FirstPersonMinCameraYaw = -89;
	FirstPersonMaxCameraYaw = 89;
	FirstPersonMinCameraRoll = 0;
	FirstPersonMaxCameraRoll = 0;

	HeadSocket = "Item_Head";

	bFaceLockedTarget = false;

	CurrentCameraComponent = nullptr;
	ThirdPersonCamera = nullptr;
	FirstPersonCamera = nullptr;
	ShoulderCamera = nullptr;
	TargetLockCamera = nullptr;
}

void AAdvKitCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAdvKitCharacter, InventoryManager);
	DOREPLIFETIME(AAdvKitCharacter, ActionManager);
	DOREPLIFETIME(AAdvKitCharacter, RepAnimMontage);
	DOREPLIFETIME(AAdvKitCharacter, Health);
	DOREPLIFETIME(AAdvKitCharacter, MaxHealth);
	DOREPLIFETIME(AAdvKitCharacter, LocationInZone);
}

const struct FAdvKitMovementProperties* AAdvKitCharacter::GetAdvMovementProperties() const
{ 
	const UAdvKitCharacterMovementComponent* AdvMoveComp = GetAdvMovementComponent();
	if (!AdvMoveComp)
	{
		//No properties without movement component
		return nullptr;
	}
	
	return AdvMoveComp->GetAdvMovementProperties();
}

class UAdvKitCharacterMovementComponent* AAdvKitCharacter::GetAdvMovementComponent() const
{
	return Cast<UAdvKitCharacterMovementComponent>(GetMovementComponent());
}

FRotator AAdvKitCharacter::GetBaseAimRotation() const
{
	//Aim is modified to look at target
	if (bLockTarget && CurrentTarget)
	{
		FVector LocationDifference = CurrentTarget->GetActorLocation() - GetActorLocation();
		return (LocationDifference.Rotation().Quaternion()).Rotator();
	}

	return Super::GetBaseAimRotation();
}

UAdvKitCharacterAction* AAdvKitCharacter::GetActiveAction() const
{
	if (!IsValid(ActionManager))
	{
		//No action without action manager
		return nullptr;
	}

	return ActionManager->GetActiveAction();
}

void AAdvKitCharacter::NextItem()
{
	if (!IsValid(InventoryManager))
	{
		return;
	}

	InventoryManager->NextItem();
}

void AAdvKitCharacter::PreviousItem()
{
	if (!IsValid(InventoryManager))
	{
		return;
	}

	InventoryManager->PreviousItem();
}

FRotator AAdvKitCharacter::GetAdjustedAimFor(AAdvKitWeapon* Weapon, FVector FireLocation)
{
	//Without a controller only the character can be used
	if (Controller == NULL || Role < ROLE_Authority)
	{
		return GetBaseAimRotation();
	}

	//Always look at locked target
	if (bLockTarget && CurrentTarget)
	{
		return (CurrentTarget->GetActorLocation() - GetWeaponStartTraceLocation(Weapon)).Rotation();
	}

	return GetBaseAimRotation();
}

FVector AAdvKitCharacter::GetWeaponStartTraceLocation(AAdvKitWeapon* Weapon)
{
	//If a controller is present use it for view location
	if (Controller)
	{
		FVector Location;
		FRotator Rotation;
		Controller->GetPlayerViewPoint(Location, Rotation);
		return Location;
	}

	//Locking on target means the character is the start for a weapon trace
	if (IsThirdPerson() || bLockTarget)
	{
		return GetMesh()->GetSocketLocation(HeadSocket);
	}

	return GetPawnViewLocation();
}


float AAdvKitCharacter::GetMaximumTargetDistance()
{
	return MaxTargetDistance;
}

TArray<AAdvKitTargetPoint*> AAdvKitCharacter::GetAllAvailableTargets()
{
	const float CurTime = GetWorld()->TimeSeconds;
	
	//How long ago the last render has to be in order to considerer it invisible.
	//Originally this was supposed to be since the last frame, e.g. DeltaSeconds, however 
	//due to float inaccuracies it is doubled to avoid flickering.
	const float DeltaSeconds = GetWorld()->DeltaTimeSeconds * 2;
	
	TArray<AAdvKitTargetPoint*> ResultArray;

	float MaxDistance = GetMaximumTargetDistance();

	//Go through all target actors
	for (TActorIterator<AAdvKitTargetPoint> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AAdvKitTargetPoint* TempTarget = Cast<AAdvKitTargetPoint>(*ActorItr);

		//Check if target actor is attached to another actor to avoid targeting loose traget points
		if (!TempTarget || !TempTarget->IsValidTarget())
		{
			continue;
		}

		auto TargetedActor = TempTarget->GetTargetedActor();
		if (!TargetedActor || TargetedActor == this)
		{
			continue;
		}

		//Check if the target is close enough
		if (FVector::Dist(GetActorLocation(), TempTarget->GetActorLocation()) > MaxDistance)
		{
			continue;
		}

		//Only include visible targets
		float LastRenderDelta = CurTime - TargetedActor->GetLastRenderTime();
		if (LastRenderDelta > DeltaSeconds) //TODO this is a nasty hack...?
		{
			continue;
		}

		ResultArray.AddUnique(TempTarget);
	}

	return ResultArray;
}

AAdvKitTargetPoint* AAdvKitCharacter::GetBestTarget(const TArray<AAdvKitTargetPoint*> &AvailableTargets)
{
	float ClosestDistance = GetMaximumTargetDistance();
	AAdvKitTargetPoint* ClosestTarget = NULL;

	//Find closest target
	for (AAdvKitTargetPoint* TempTarget : AvailableTargets)
	{
		float Distance = FVector::Dist(GetActorLocation(), TempTarget->GetActorLocation());
		if ((ClosestTarget && Distance<ClosestDistance) || !ClosestTarget)
		{
			ClosestDistance = Distance;
			ClosestTarget = TempTarget;
		}
	}

	return ClosestTarget;
}

void AAdvKitCharacter::SetTarget(AAdvKitTargetPoint* NewTarget, bool bLock)
{
	//Don't do anything if new values are already set
	if (CurrentTarget == NewTarget && bLock == bLockTarget)
	{
		return;
	}
	
	CurrentTarget = NewTarget;

	//Cannot lock to nothing
	if (!CurrentTarget)
	{
		bLock = false;

		//Revert camera
		if (CameraType == EAdvKitCameraType::TargetLocked)
		{
			SetCameraModeByComponent(LastCameraComponent);
		}

		return;
	}

	//Lock is already set to what it should be
	if (bLock == bLockTarget)
	{
		return;
	}

	bLockTarget = bLock;

	//Lock the target
	if (bLock)
	{
		SetCameraMode(EAdvKitCameraType::TargetLocked);
		return;
	}

	//Unlock the target
	if (CameraType == EAdvKitCameraType::TargetLocked)
	{
		SetCameraModeByComponent(LastCameraComponent);
	}
}

bool AAdvKitCharacter::StartAction(TSubclassOf<class UAdvKitCharacterAction> ActionClass, class UAdvKitCharacterAction_Arguments* Arguments)
{
	if (!IsValid(ActionManager))
	{
		return false;
	}

	return ActionManager->StartAction(ActionClass, Arguments).IsSuccess();
}

bool AAdvKitCharacter::StartTransitionActionTo(EAdvKitMovementMode NewMode, class UAdvKitCharacterAction_Arguments* Arguments)
{
	UAdvKitCharacterMovementComponent* AdvMoveComp = Cast<UAdvKitCharacterMovementComponent>(GetMovementComponent());
	if (!AdvMoveComp)
	{
		return false;
	}

	//Start the physics transition
	return StartAction(
		UAdvKitCA_PhysicsTransition::StaticClass(),
		UAdvKitCA_PhysicsTransition::MakeArguments(
			AdvMoveComp->GetAdvMovementMode(),
			NewMode,
			Arguments
			)
		);
}

void AAdvKitCharacter::StartFire(uint8 FireMode)
{
	//Point and click
	APlayerController* PlayerController = Cast<APlayerController>(this->Controller);
	if (IsValid(PlayerController) && PlayerController->bShowMouseCursor)
	{
		FVector Start;
		FVector Direction;
		PlayerController->DeprojectMousePositionToWorld(Start, Direction);

		FVector End = Start + Direction * PointAndClickTraceDistance;

		FHitResult hitresult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		FCollisionObjectQueryParams ObjectQueryParams;
		
		//Try to hit an actor
		if (!GetWorld()->LineTraceSingleByObjectType(hitresult, Start, End, ObjectQueryParams, Params))
		{
			return;
		}

		//Check if actor is usable
		AAdvKitUsable* Useable = Cast<AAdvKitUsable>(hitresult.GetActor());
		if (!IsValid(Useable))
		{
			return;
		}

		//Check if usable can be used
		if (!Useable->CanBeUsedBy(this, InventoryManager->GetCurrentInventory()))
		{
			return;
		}

		//Use it
		Use(Useable, InventoryManager->GetCurrentInventory());
		return;
	}

	//Try to start firing

	//Need inventory to have weapon
	if (!InventoryManager)
	{
		return;
	}

	//Get active weapon and fire
	AAdvKitWeapon* CurrentWeapon = Cast<AAdvKitWeapon>(InventoryManager->GetCurrentInventory());
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire(FireMode);
	}
}

void AAdvKitCharacter::StopFire()
{
	//If in point and click mode weapons are not used
	APlayerController* PlayerController = Cast<APlayerController>(this->Controller);
	if (IsValid(PlayerController) && PlayerController->bShowMouseCursor)
	{
		return;
	}

	//Need inventory to have weapon
	if (!InventoryManager)
	{
		return;
	}

	//Get active weapon and stop
	AAdvKitWeapon* CurrentWeapon = Cast<AAdvKitWeapon>(InventoryManager->GetCurrentInventory());
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}


void AAdvKitCharacter::RestrictViewRotation(FRotator& DesiredLocalRotation)
{
	//View rotation is only restricted in first person mode
	if (!IsFirstPerson())
	{
		return;
	}

	//Get head orientation
	FTransform headTransform = GetMesh()->GetSocketTransform(HeadSocket, ERelativeTransformSpace::RTS_World);
	FQuat headQuat = headTransform.GetRotation();

	//Transform given rotation to local head space
	FQuat localQuat = headQuat.Inverse() * DesiredLocalRotation.Quaternion();
	FRotator localRotator = localQuat.Rotator();

	//Clamp angles of local rotation
	if (bFirstPersonClampCameraPitch && !bUseControllerRotationPitchInFirstPerson)
	{
		localRotator.Pitch = FMath::ClampAngle(localRotator.Pitch, FirstPersonMinCameraPitch, FirstPersonMaxCameraPitch);
	}

	if (bFirstPersonClampCameraYaw && !bUseControllerRotationYawInFirstPerson)
	{
		localRotator.Yaw = FMath::ClampAngle(localRotator.Yaw, FirstPersonMinCameraYaw, FirstPersonMaxCameraYaw);
	}

	if (bFirstPersonClampCameraRoll && !bUseControllerRotationRollInFirstPerson)
	{
		localRotator.Roll = FMath::ClampAngle(localRotator.Roll, FirstPersonMinCameraRoll, FirstPersonMaxCameraRoll);
	}

	//Transform back to world space
	localQuat = localRotator.Quaternion();
	DesiredLocalRotation = (headQuat * localQuat).Rotator();
}

void AAdvKitCharacter::Reload()
{
	//Need inventory to have weapon
	if (!InventoryManager)
	{
		return;
	}

	//Get active weapon and reload
	AAdvKitWeapon* CurrentWeapon = Cast<AAdvKitWeapon>(InventoryManager->GetCurrentInventory());
	if (CurrentWeapon)
	{
		CurrentWeapon->StartReload();
	}
}

void AAdvKitCharacter::Use(class AAdvKitUsable* Usable /*= NULL*/, class AAdvKitInventoryItem* WithItem /*= NULL*/)
{
	//Cannot use nothing
	if (!Usable)
	{
		return;
	}

	//Notify server of use
	if (Role < ROLE_Authority)
	{
		ServerUse(Usable, WithItem);
		return;
	}

	//Check if object can be used
	if (!Usable->CanBeUsedBy(this, WithItem))
	{
		return;
	}

	//Use object
	Usable->UsedBy(this, WithItem);
}


bool  AAdvKitCharacter::ServerUse_Validate(class AAdvKitUsable* Useable, class AAdvKitInventoryItem* WithItem)
{
	return true;
}

void AAdvKitCharacter::ServerUse_Implementation(class AAdvKitUsable* Useable, class AAdvKitInventoryItem* WithItem)
{
	Use(Useable, WithItem);
}

void AAdvKitCharacter::NextCamera()
{
	//Cycle through camera types
	switch (CameraType)
	{
	case EAdvKitCameraType::FirstPerson:
		SetCameraMode(EAdvKitCameraType::ThirdPerson);
		break;
	case EAdvKitCameraType::Shoulder:
		SetCameraMode(EAdvKitCameraType::FirstPerson);
		break;
	case EAdvKitCameraType::ThirdPerson:
		SetCameraMode(EAdvKitCameraType::Shoulder);
		break;
	case EAdvKitCameraType::TargetLocked:
		SetCameraMode(EAdvKitCameraType::TargetLocked);
		break;
	}
}

void AAdvKitCharacter::SetFirstPersonEnabled(bool bEnabled)
{
	//Toggle on server
	if (Role < ROLE_Authority)
	{
		SetFirstPersonEnabledServer(bEnabled);
	}

	if (bEnabled)
	{
		SetCameraMode(EAdvKitCameraType::FirstPerson);
	}
	else
	{
		SetCameraMode(EAdvKitCameraType::ThirdPerson);
	}
}

void AAdvKitCharacter::SetCameraMode(EAdvKitCameraType NewMode)
{
	//Set on server
	if (Role == ROLE_AutonomousProxy)
	{
		SetCameraModeServer(NewMode);
	}

	//Flag for third person
	bool bOrientToMovement = false;
	CameraType = NewMode;

	switch (NewMode)
	{
	case EAdvKitCameraType::FirstPerson:
		SetActiveCameraComponent(FirstPersonCamera);
		break;
	case EAdvKitCameraType::ThirdPerson:
		SetActiveCameraComponent(ThirdPersonCamera);
		bOrientToMovement = true;
		break;
	case EAdvKitCameraType::Shoulder:
		SetActiveCameraComponent(ShoulderCamera);
		break;
	case EAdvKitCameraType::TargetLocked:
		SetActiveCameraComponent(TargetLockCamera);
		bOrientToMovement = true;
		break;
	}

	// Configure character movement
	if (GetCharacterMovement()) //replicated pawns don't have this initialized
	{
		GetCharacterMovement()->bOrientRotationToMovement = bOrientToMovement;
	}
}

bool AAdvKitCharacter::SetActiveCameraComponent(UCameraComponent* NewCameraComponent)
{
	if (!NewCameraComponent)
	{
		return false;
	}

	//Switch camera components and adjust interpolation
	LastCameraComponent = CurrentCameraComponent ? CurrentCameraComponent : NewCameraComponent;
	CurrentCameraComponent = NewCameraComponent;
	CameraLerpAlpha = 1 - CameraLerpAlpha;

	return true;
}

void AAdvKitCharacter::SetCameraModeByComponent(UCameraComponent* NewCameraComponent)
{
	//Figure out which camera is for which mode and set that

	if (ThirdPersonCamera && NewCameraComponent == ThirdPersonCamera)
	{
		SetCameraMode(EAdvKitCameraType::ThirdPerson);
	}
	else if (FirstPersonCamera && NewCameraComponent == FirstPersonCamera)
	{
		SetCameraMode(EAdvKitCameraType::FirstPerson);
	}
	else if (TargetLockCamera && NewCameraComponent == TargetLockCamera)
	{
		SetCameraMode(EAdvKitCameraType::TargetLocked);
	}
	else if (ShoulderCamera && NewCameraComponent == ShoulderCamera)
	{
		SetCameraMode(EAdvKitCameraType::Shoulder);
	}
}

bool AAdvKitCharacter::SetCameraModeServer_Validate(EAdvKitCameraType NewMode)
{
	return true;
}

void AAdvKitCharacter::SetCameraModeServer_Implementation(EAdvKitCameraType NewMode)
{
	SetCameraMode(NewMode);
}

EAdvKitCameraType AAdvKitCharacter::GetCameraMode()
{
	return CameraType;
}

bool AAdvKitCharacter::SetFirstPersonEnabledServer_Validate(bool bEnabled)
{
	return true;
}

void AAdvKitCharacter::SetFirstPersonEnabledServer_Implementation(bool bEnabled)
{
	SetFirstPersonEnabled(bEnabled);
}


bool AAdvKitCharacter::EnterClosestZone()
{
	UAdvKitCharacterMovementComponent* AdvMovementComp = Cast<UAdvKitCharacterMovementComponent>(GetMovementComponent());
	if (!AdvMovementComp)
	{
		return false;
	}

	//Get all potential zones
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AAdvKitZone::StaticClass());
	for (AActor* TempActor : OverlappingActors)
	{
		//Filter for zone
		AAdvKitZone* OverlappedZone = Cast<AAdvKitZone>(TempActor);
		if (!OverlappedZone)
		{
			continue;
		}

		//Try to enter zone
		if (AdvMovementComp->TryToTransition(
			OverlappedZone,
			GetActorLocation(), GetActorForwardVector()
			))
		{
			return true;
		}
	}

	//No zone was entered
	return false;
}

void AAdvKitCharacter::SetZone(AAdvKitZone* NewZone, bool bSnap)
{
	if (Role == ROLE_AutonomousProxy)
	{
		SetZoneServer(NewZone, bSnap);
		return;
	}


	auto* AdvMoveComp = GetAdvMovementComponent();
	check(AdvMoveComp);

	//Cannot set location without zone
	if (!NewZone)
	{
		SetZoneLocation(nullptr, bSnap);
		return;
	}

	//Get zone location
	FVector HalfExtent = AdvMoveComp->GetHalfExtentForZone(NewZone);
	LocationInZone = NewZone->GetClosestZoneLocation(GetActorLocation(), HalfExtent, GetActorRotation());
	SetZoneLocation(LocationInZone, bSnap);
}

bool AAdvKitCharacter::SetZoneServer_Validate(AAdvKitZone* NewZone, bool bSnap)
{
	return true;
}

void AAdvKitCharacter::SetZoneServer_Implementation(AAdvKitZone* NewZone, bool bSnap)
{
	SetZone(NewZone, bSnap);
}

class AAdvKitZone* AAdvKitCharacter::GetZone() const
{
	if (!LocationInZone)
	{
		return nullptr;
	}

	return LocationInZone->Zone;
	//return Cast<AAdvKitZone>(GetMovementBaseActor(this));
	//return Cast<AAdvKitZone>(GetAttachParentActor());
}

void AAdvKitCharacter::SetZoneLocation(UAdvKitZoneLocation* NewLocation, bool bSnap)
{
	if (!NewLocation)
	{
		SetBase(nullptr);
		LocationInZone = NewLocation;
		return;
	}

	if (!NewLocation->Zone)
	{
		SetBase(nullptr);
		LocationInZone = NewLocation;
		return;
	}

	//Set new base variables
	LocationInZone = NewLocation;
	SetBase(Cast<UPrimitiveComponent>(NewLocation->Zone->GetRootComponent()));

	if (!bSnap)
	{
		return;
	}

	auto* AdvMoveComp = GetAdvMovementComponent();
	check(AdvMoveComp);
	
	//Place character according to zone
	const FVector DesiredOffset = AdvMoveComp->GetDesiredOffsetFromZone(LocationInZone->Zone, LocationInZone);
	SetActorLocation(LocationInZone->GetGlobalPosition() + DesiredOffset);
	SetActorRotation(AdvMoveComp->GetDesiredRotationInZone(LocationInZone->Zone, LocationInZone));
}

bool AAdvKitCharacter::SetZoneLocationServer_Validate(UAdvKitZoneLocation* NewLocation, bool bSnap)
{
	return true;
}

void AAdvKitCharacter::SetZoneLocationServer_Implementation(UAdvKitZoneLocation* NewLocation, bool bSnap)
{
	SetZoneLocation(NewLocation, bSnap);
}

UAdvKitZoneLocation* AAdvKitCharacter::GetZoneLocation()
{
	return LocationInZone;
}

bool AAdvKitCharacter::UseClosestUseable()
{
	AAdvKitUsable* ClosestUseable = NULL;
	float LastDistance = 0;

	AAdvKitInventoryItem* CurrentInventory = NULL;
	if (InventoryManager)
	{
		CurrentInventory = InventoryManager->GetCurrentInventory();
	}

	//Overlap all possible targets around the character
	TArray<struct FOverlapResult> Results;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	FCollisionObjectQueryParams QueryParams;
	if (!GetWorld()->OverlapMultiByObjectType(Results, GetActorLocation(), FQuat::Identity, QueryParams, FCollisionShape::MakeSphere(MaxTargetDistance), Params))
	{
		//Nothing overlapped
		return false;
	}

	//Filter results
	for (auto Result : Results)
	{
		AAdvKitUsable* TempUsable = Cast<AAdvKitUsable>(Result.GetActor());
		if (!TempUsable)
		{
			//No usable
			continue;
		}

		//Cannot be used
		if (!TempUsable->CanBeUsedBy(this, CurrentInventory))
		{
			continue;
		}

		//Set first result
		if (!ClosestUseable)
		{
			ClosestUseable = TempUsable;
			LastDistance = FVector::Dist(ClosestUseable->GetActorLocation(), GetActorLocation());
			continue;
		}

		//Check if current candidate is closer
		float NewDistance = FVector::Dist(TempUsable->GetActorLocation(), GetActorLocation());
		if (NewDistance < LastDistance)
		{
			ClosestUseable = TempUsable;
			LastDistance = NewDistance;
		}
	}

	if (!ClosestUseable)
	{
		//Nothing was found
		return false;
	}

	//Use found usable
	Use(ClosestUseable, CurrentInventory);
	return true;
}

void AAdvKitCharacter::SetMouseCursorEnabled(bool bEnabled)
{
	APlayerController* PlayerController = Cast<APlayerController>(this->Controller);
	if (!IsValid(PlayerController))
	{
		return;
	}

	//Set mouse cursor flag
	PlayerController->bShowMouseCursor = bEnabled;

	//Place mouse in the center of the screen
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->Player);
	FViewport* Viewport = LocalPlayer->ViewportClient->Viewport;
	FIntPoint ScreenSize = Viewport->GetSizeXY();
	Viewport->SetMouse(ScreenSize.X / 2, ScreenSize.Y / 2);

	//Fix to prevent the cursor being restricted by the window border, thus preventing proper rotation by mouse movement
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
}


bool AAdvKitCharacter::IsFirstPerson() const
{
	return CameraType == EAdvKitCameraType::FirstPerson;
}

bool AAdvKitCharacter::IsThirdPerson() const
{
	return CameraType == EAdvKitCameraType::ThirdPerson;
}

bool AAdvKitCharacter::IsShoulderCam() const
{
	return CameraType == EAdvKitCameraType::Shoulder;
}

FRotator AAdvKitCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}


void AAdvKitCharacter::Dodge_Implementation()
{
	//Dodge forward
	FVector DodgeDirection = GetActorForwardVector();

	auto AdvMovementComponent = Cast<UAdvKitCharacterMovementComponent>(GetMovementComponent());
	if (AdvMovementComponent)
	{
		//Get dodge direction by current acceleration
		FVector CurrentAcceleration = AdvMovementComponent->GetCurrentAcceleration();
		if (!CurrentAcceleration.IsNearlyZero())
		{
			DodgeDirection = CurrentAcceleration;
		}
		//Get direction away from target
		else
		{
			if (bLockTarget)
			{
				DodgeDirection = -GetActorForwardVector();
			}
		}
	}

	StartAction(UAdvKitCA_Dodge::StaticClass(), UAdvKitCA_Dodge::MakeArguments(DodgeDirection));
}

bool AAdvKitCharacter::IsDodging_Implementation() const
{
	auto Action = GetActiveAction();
	if (!Action)
	{
		return false;
	}

	return Action->IsA(UAdvKitCA_Dodge::StaticClass());
}

bool AAdvKitCharacter::DodgeServer_Validate()
{
	return true;
}


void AAdvKitCharacter::DodgeServer_Implementation()
{
	Dodge();
}

void AAdvKitCharacter::AddConditionalMovementInput(FVector WorldDirection, FVector LocalDirection, float ScaleValue, bool bForce)
{
	FVector Direction = WorldDirection;

	auto MoveComp = GetAdvMovementComponent();
	if (MoveComp)
	{
		switch (MoveComp->GetAdvMovementMode())
		{
		case EAdvKitMovementMode::ClimbingWall:
		case EAdvKitMovementMode::ClimbingLedge:
		case EAdvKitMovementMode::ClimbingLadder:
			Direction = GetActorRotation().RotateVector(LocalDirection);
			break;
		default:
			break;
		}
	}

	AddMovementInput(Direction, ScaleValue, bForce);
}

void AAdvKitCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	bool bHasAdvCamera = false;

	//Find and assign all camera components
	TArray<UActorComponent*> CamComponents = GetComponentsByClass(UCameraComponent::StaticClass());
	for (auto Comp : CamComponents)
	{
		auto CampComp = Cast<UCameraComponent>(Comp);
		if (!CampComp)
		{
			continue;
		}

		if (CampComp->ComponentHasTag("Shoulder"))
		{
			bHasAdvCamera = true;
			ShoulderCamera = CampComp;
		}

		if (CampComp->ComponentHasTag("First Person"))
		{
			bHasAdvCamera = true;
			FirstPersonCamera = CampComp;
		}

		if (CampComp->ComponentHasTag("Third Person"))
		{
			bHasAdvCamera = true;
			ThirdPersonCamera = CampComp;
		}

		if (CampComp->ComponentHasTag("Target Lock"))
		{
			bHasAdvCamera = true;
			TargetLockCamera = CampComp;
		}
	}

	if (!bHasAdvCamera)
	{
		bFindCameraComponentWhenViewTarget = true;
	}

	SetCameraMode(CameraType);

	if (Role < ROLE_Authority)
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = this;
	SpawnParameters.Owner = this;

	//Create action manager
	ActionManager = GetWorld()->SpawnActor<class AAdvKitCharacterActionManager>(ActionManagerClass, SpawnParameters);
	if (ActionManager)
	{
		ActionManager->AttachRootComponentToActor(this, NAME_None, EAttachLocation::SnapToTarget);
	}

	//Create inventory manager
	InventoryManager = GetWorld()->SpawnActor<class AAdvKitInventoryManager>(InventoryManagerClass, SpawnParameters);
	if (InventoryManager)
	{
		InventoryManager->AttachRootComponentToActor(this, NAME_None, EAttachLocation::SnapToTarget);
	}
}

void AAdvKitCharacter::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);

	if (Role <= ROLE_Authority)
	{
		return;
	}

	if (InventoryManager)
	{
		InventoryManager->Destroy();
	}

	if (ActionManager)
	{
		ActionManager->Destroy();
	}
}

void AAdvKitCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Update camera alpha
	CameraLerpAlpha = FMath::Clamp<float>(CameraLerpAlpha + CameraLerpSpeed*DeltaSeconds, 0, 1);

	//Set active target automatically if not locked
	if (!bLockTarget)
	{
		auto* Target = GetBestTarget(GetAllAvailableTargets());
		SetTarget(Target, false);
	}
}

FVector AAdvKitCharacter::GetPawnViewLocation() const
{
	//Camera not set, use base implementation
	if (!CurrentCameraComponent)
	{
		return Super::GetPawnViewLocation();
	}

	//Interpolate between last and new camera
	if (LastCameraComponent && CameraLerpAlpha<1.0f)
	{
		return FMath::Lerp<FVector>(LastCameraComponent->GetComponentLocation(), CurrentCameraComponent->GetComponentLocation(), FMath::InterpEaseInOut<float>(0.0f, 1.0f, CameraLerpAlpha, CameraLerpExponent));
	}

	//Current component is completely active
	return CurrentCameraComponent->GetComponentLocation();
}

FRotator AAdvKitCharacter::GetViewRotation() const
{
	FRotator SuperRotation = Super::GetViewRotation();

	//Without target lock there is no need to modify the view rotation
	if (TargetLockCamera == nullptr)
	{
		return SuperRotation;
	}

	//Lerp from current camera to target lock view
	if (CurrentCameraComponent == TargetLockCamera)
	{
		return FQuat::FastLerp(SuperRotation.Quaternion(), CurrentCameraComponent->GetComponentQuat(), FMath::InterpEaseInOut<float>(0.0f, 1.0f, CameraLerpAlpha, CameraLerpExponent)).Rotator();
	}

	//Lerp from target lock view to current camera
	if (LastCameraComponent == TargetLockCamera)
	{
		return FQuat::FastLerp(LastCameraComponent->GetComponentQuat(), SuperRotation.Quaternion(), FMath::InterpEaseInOut<float>(0.0f, 1.0f, CameraLerpAlpha, CameraLerpExponent)).Rotator();
	}

	return SuperRotation;
}

void AAdvKitCharacter::NotifyActorBeginOverlap(class AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (!bEnterZonesFromOverlapAutomatically)
	{
		return;
	}

	//Check if overlapped actor is a zone
	AAdvKitZone* OverlappedZone = Cast<AAdvKitZone>(OtherActor);
	if (CanEnterZone(OverlappedZone))
	{
		EnterZone(OverlappedZone);
	}
}

void AAdvKitCharacter::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (!bEnterZonesFromHitAutomatically)
	{
		return;
	}

	//Ignore hits during an action
	if (GetActiveAction() != nullptr)
	{
		return;
	}

	//Enter hit zone
	AAdvKitZone *NewZone = Cast<AAdvKitZone>(Hit.Actor.Get());
	if (CanEnterZone(NewZone))
	{
		EnterZone(NewZone);
	}
}

bool AAdvKitCharacter::CanEnterZone(AAdvKitZone* Zone)
{
	if (!Zone)
	{
		return false;
	}

	//Already in a zone
	if (GetZone())
	{
		return false;
	}

	auto AdvMoveComp = GetAdvMovementComponent();
	if (AdvMoveComp)
	{
		auto Properties = AdvMoveComp->GetAdvMovementProperties();
		auto Physics = Zone->GetPhysics();

		switch (Physics)
		{
		case EAdvKitMovementMode::ClimbingCeiling:
			if (!Properties->bCanClimbCeiling)
			{
				return false;
			}
			break;
		case EAdvKitMovementMode::ClimbingLadder:
			if (!Properties->bCanClimbLadder)
			{
				return false;
			}
			break;
		case EAdvKitMovementMode::ClimbingLedge:
			if (!Properties->bCanClimbLedge)
			{
				return false;
			}
			break;
		case EAdvKitMovementMode::ClimbingWall:
			if (!Properties->bCanClimbWall)
			{
				return false;
			}
			break;
		case EAdvKitMovementMode::WalkingBalance:
			if (!Properties->bCanWalkBalance)
			{
				return false;
			}
			break;
		case EAdvKitMovementMode::WalkingTightspace:
			if (!Properties->bCanWalkTightSpace)
			{
				return false;
			}
			break;
		}
	}

	if (!Zone->IsCharacterSupported(this))
	{
		return false;
	}

	return true;
}

bool AAdvKitCharacter::EnterZone(AAdvKitZone* Zone)
{
	if (Role == ROLE_AutonomousProxy)
	{
		EnterZoneServer(Zone);
		return true;
	}

	return StartTransitionActionTo(Zone->GetPhysics(), UAdvKitCA_ZoneTransition::MakeArguments(Zone));
}


bool AAdvKitCharacter::EnterZoneServer_Validate(AAdvKitZone* NewZone)
{
	return true;
}

void AAdvKitCharacter::EnterZoneServer_Implementation(AAdvKitZone* NewZone)
{
	EnterZone(NewZone);
}


bool AAdvKitCharacter::ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const 
{
	if (!Super::ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser))
	{
		return false;
	}

	if (bInvincibleDuringDodge && IsDodging())
	{
		return false;
	}

	return true;
}

float AAdvKitCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (!ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser))
	{
		return 0.f;
	}

	float ActualDamage = BlockDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	ActualDamage = Super::TakeDamage(ActualDamage, DamageEvent, EventInstigator, DamageCauser);

	//Avoid calling the died method multiple times
	bool bWasDeadPrior = Health <= 0;

	Health -= ActualDamage;

	if (Health <= 0 && !bWasDeadPrior)
	{
		Died(EventInstigator, DamageEvent, GetActorLocation());
	}

	return ActualDamage;
}

float AAdvKitCharacter::BlockDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	float ActualDamage = Damage;

	//Call block method depending on damage event class
	UDamageType const* const DamageTypeCDO = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent* const PointDamageEvent = (FPointDamageEvent*)&DamageEvent;
		ActualDamage = BlockPointDamage(ActualDamage, DamageTypeCDO, PointDamageEvent->HitInfo.ImpactPoint, PointDamageEvent->HitInfo.ImpactNormal, PointDamageEvent->HitInfo.Component.Get(), PointDamageEvent->HitInfo.BoneName, PointDamageEvent->ShotDirection, EventInstigator, DamageCauser);
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		FRadialDamageEvent* const RadialDamageEvent = (FRadialDamageEvent*)&DamageEvent;
		FHitResult const& Hit = (RadialDamageEvent->ComponentHits.Num() > 0) ? RadialDamageEvent->ComponentHits[0] : FHitResult();
		ActualDamage = BlockRadialDamage(ActualDamage, DamageTypeCDO, RadialDamageEvent->Origin, Hit, EventInstigator, DamageCauser);
	}

	//Block any remaining damage
	if (ActualDamage != 0.f)
	{
		ActualDamage = BlockAnyDamage(ActualDamage, DamageTypeCDO, EventInstigator, DamageCauser);
	}

	//In case the character holds a weapon have it be possible for the weapon to block damage
	if (!InventoryManager)
	{
		return ActualDamage;
	}

	AAdvKitWeapon* Weapon = Cast<AAdvKitWeapon>(InventoryManager->GetCurrentInventory());
	if (!Weapon)
	{
		return ActualDamage;
	}

	return Weapon->BlockDamage(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
}

float AAdvKitCharacter::BlockAnyDamage_Implementation(float DamageReceived, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//Dummy Implementation, does nothing
	return DamageReceived;
}

float AAdvKitCharacter::BlockRadialDamage_Implementation(float DamageReceived, const class UDamageType* DamageType, FVector Origin, const struct FHitResult& HitInfo, class AController* InstigatedBy, AActor* DamageCauser)
{
	//Dummy Implementation, does nothing
	return DamageReceived;
}

float AAdvKitCharacter::BlockPointDamage_Implementation(float DamageReceived, const class UDamageType* DamageType, FVector HitLocation, FVector HitNormal, class UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, class AController* InstigatedBy, AActor* DamageCauser)
{
	//Dummy Implementation, does nothing
	return DamageReceived;
}

void AAdvKitCharacter::Jump()
{
	auto* AdvMoveComp = GetAdvMovementComponent();
	if (!AdvMoveComp)
	{
		Super::Jump();
		return;
	}

	//Try to jump in zone
	if (AdvMoveComp->JumpUpZone())
	{
		return;
	}

	Super::Jump();
}


void AAdvKitCharacter::Crouch(bool bClientSimulation)
{
	auto* AdvMoveComp = GetAdvMovementComponent();
	if (!AdvMoveComp)
	{
		Super::Crouch(bClientSimulation);
		return;
	}

	if (AdvMoveComp->LetGoOfZone())
	{
		return;
	}

	Super::Crouch(bClientSimulation);
}


void AAdvKitCharacter::FaceRotation(FRotator NewControlRotation, float DeltaSeconds)
{
	if (GetActiveAction())
	{
		return;
	}

	FRotator CurrentRotation = GetActorRotation();

	if (!bUseControllerRotationPitch && !(bUseControllerRotationPitchInFirstPerson && IsFirstPerson()))
	{
		NewControlRotation.Pitch = CurrentRotation.Pitch;
		NewControlRotation.Pitch = 0; //TODO hotfix, make a flag for this (e.g. constrain pitch to plane?)
	}
		
	if (!bUseControllerRotationYawInFirstPerson || IsThirdPerson() || bLockTarget)
	{
		NewControlRotation.Yaw = CurrentRotation.Yaw;
	}

	if (!bUseControllerRotationRoll && !(bUseControllerRotationRollInFirstPerson && IsFirstPerson()))
	{
		NewControlRotation.Roll = CurrentRotation.Roll;
		NewControlRotation.Roll = 0; //TODO hotfix, make a flag for this (e.g. constrain roll to plane?)
	}

	if (bFaceLockedTarget && bLockTarget && CurrentTarget)
	{
		FVector LocationDifference = CurrentTarget->GetActorLocation() - GetActorLocation();
		LocationDifference.Z = 0;
		NewControlRotation = LocationDifference.Rotation();
	}

	SetActorRotation(NewControlRotation);
}

bool AAdvKitCharacter::CanJumpInternal_Implementation() const
{
	if (GetZone())
	{
		return true;
	}

	return Super::CanJumpInternal_Implementation();
}

void AAdvKitCharacter::SetBase(UPrimitiveComponent* NewBase, const FName BoneName, bool bNotifyActor)
{
	Super::SetBase(NewBase, BoneName, bNotifyActor);

	if (!IsValid(NewBase) || !IsValid(NewBase->GetOwner()) || !NewBase->GetOwner()->IsA(AAdvKitZone::StaticClass()))
	{
		for (auto Channel : CollisionChannelsToIgnoreInZones)
		{
			GetCapsuleComponent()->SetCollisionResponseToChannel(Channel.GetValue(), ECollisionResponse::ECR_Block);
		}
		GetAdvMovementComponent()->MoveComponentFlags = MOVECOMP_NoFlags;
		return;
	}

	for (auto Channel : CollisionChannelsToIgnoreInZones)
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(Channel.GetValue(), ECollisionResponse::ECR_Ignore);
	}
	GetAdvMovementComponent()->MoveComponentFlags = MOVECOMP_IgnoreBases;
}

bool AAdvKitCharacter::IsAlive_Implementation()
{
	return Health > 0.0f;
}

void AAdvKitCharacter::Died_Implementation(AController* Killer, struct FDamageEvent const& DamageEvent, FVector HitLocation)
{
	if (!InventoryManager)
	{
		return;
	}

	InventoryManager->OwnerDied();
}

float AAdvKitCharacter::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	//Replicate to server
	if (Role == ROLE_AutonomousProxy)
	{
		PlayAnimMontageServer(AnimMontage, InPlayRate, StartSectionName);
	}

	//Set parameters
	if (Role == ROLE_Authority || Role == ROLE_AutonomousProxy)
	{
		RepAnimMontage.SetNewMontage(AnimMontage, InPlayRate, StartSectionName);
	}

	//Play the anim montage
	return Super::PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
}

void AAdvKitCharacter::StopAnimMontage(class UAnimMontage* AnimMontage)
{
	Super::StopAnimMontage(AnimMontage);
}

void AAdvKitCharacter::OnRep_RepAnimMontage()
{
	//Montage got repliated, play it
	PlayAnimMontage(RepAnimMontage.Montage, RepAnimMontage.PlayRate, RepAnimMontage.StartSectionName);
}

bool AAdvKitCharacter::PlayAnimMontageServer_Validate(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	return true;
}

void AAdvKitCharacter::PlayAnimMontageServer_Implementation(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
}
