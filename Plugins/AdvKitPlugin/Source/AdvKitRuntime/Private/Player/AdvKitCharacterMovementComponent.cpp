// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "Player/AdvKitOrientationComponent.h"
#include "Player/AdvKitTraceUtilityComponent.h"
#include "Player/AdvKitCharacter.h"
#include "Player/AdvKitCharacterActionManager.h"

#include "Environment/AdvKitZone.h"
#include "Environment/AdvKitZoneLocation.h"

#include "Environment/AdvKitTransitionComponent.h"
#include "Actions/Movement/AdvKitCharacterActionPhysics.h"


#include "Actions/Movement/AdvKitCA_ZoneTransition.h"

//Begin CharacterMovementComponent.cpp / Wrapper
#include "Navigation/PathFollowingComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogAdvKitCharacterMovement, Log, All);

// MAGIC NUMBERS from CharacterMovementComponent.cpp, works for epic, so it whould work for this
const float MAX_STEP_SIDE_Z = 0.08f;	// maximum z value for the normal on the vertical side of steps
const float SWIMBOBSPEED = -80.f;

//End CharacterMovementComponent.cpp  / Wrapper

#if WITH_EDITOR

FString GetMovementModeDisplayName(EAdvKitMovementMode EnumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EAdvKitMovementMode"), true);
	if (!EnumPtr) return FString("Invalid");

	return EnumPtr->GetEnumName((int32)EnumValue);
}
#else

FString GetMovementModeDisplayName(EAdvKitMovementMode EnumValue)
{
	return FString("Error: Editor Only");
}

#endif 


UAdvKitCharacterMovementComponent::UAdvKitCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NavAgentProps.bCanCrouch = true;
	NavAgentProps.bCanSwim = true;

	JumpDistanceLedgeLadderHorizontal = 160.0f;

	LedgeJumpDistanceVertical = 180;
	LedgeJumpDistanceHorizontal = 160;

	LedgeSmallHeightOffset = 2.0f;
	LedgeDownTraceDistance = 8.0f;
	LedgePrecognitionDistance = 30;
	
	MaxLedgeClimbSpeed = 100.0;
	LedgeOffset = FVector(33, 0, -63);

	LadderJumpDistanceHorizontal = 150;
	MaxLadderClimbSpeed = 150;
	LadderOffset = FVector(25, 0, 8);

	TightSpaceOffset = FVector(-10, 0, -5);
	MaxTightSpaceWalkSpeed = 50;

	MaxBalanceWalkSpeed = 30;
	BalanceRotationRate = 360;
	BalanceOffset = FVector(0, 0, -5);

	WallOffset = FVector(31, 0, 0);
	MaxWallClimbSpeed = 50;

	MaxCeilingClimbSpeed = 125;
	CeilingMinTransitionDot = 0.8f;
	CeilingOffset = FVector(100, 0, 0);

	BalanceChangeTime = 1;
	MaxBalance = 1;
	CounterBalanceMultiplier = 1.5f;
	MinCounterBalanceDot = 0.25f;
	MaxBalanceTurnAroundDot = 0.7f;

	MinBalanceChangePerSecond = 0.8f;
	MaxBalanceChangePerSecond = 1.0f;

	bIgnoreBaseRotation = false;
}

void UAdvKitCharacterMovementComponent::SetMovementVariables(FVector NewVelocity, FVector NewAcceleration)
{
	if (!Acceleration.IsNearlyZero())
	{
		LastNonZeroAcceleration = Acceleration;
	}

	Acceleration = NewAcceleration;
	Velocity = NewVelocity;
}

//TODO look at StopActiveMovement
void UAdvKitCharacterMovementComponent::ZeroMovementVariables()
{
	SetMovementVariables(FVector::ZeroVector, FVector::ZeroVector);
}


void UAdvKitCharacterMovementComponent::TickComponent(float DeltaSeconds, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	SetMovementVariables(Velocity, Acceleration); //Update movement variables

	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);
}

float UAdvKitCharacterMovementComponent::GetMaxSpeed() const
{
	switch (GetAdvMovementMode())
	{
	case EAdvKitMovementMode::ClimbingLedge:
		return MaxLedgeClimbSpeed;
	case EAdvKitMovementMode::ClimbingWall:
		return MaxWallClimbSpeed;
	case EAdvKitMovementMode::ClimbingLadder:
		return MaxLadderClimbSpeed;
	case EAdvKitMovementMode::ClimbingCeiling:
		return MaxCeilingClimbSpeed;
	case EAdvKitMovementMode::WalkingTightspace:
		return MaxTightSpaceWalkSpeed;
	case EAdvKitMovementMode::WalkingBalance:
		return MaxBalanceWalkSpeed;
	}

	return Super::GetMaxSpeed();
}

void UAdvKitCharacterMovementComponent::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	Super::SetUpdatedComponent(NewUpdatedComponent);
	AdvKitCharacterOwner = Cast<AAdvKitCharacter>(PawnOwner);
}

void UAdvKitCharacterMovementComponent::SetAdvMovementMode(EAdvKitMovementMode NewMovementMode, uint8 NewCustomMode)
{
	switch (NewMovementMode)
	{
	//Wrap old movement modes
	case EAdvKitMovementMode::None:
		SetMovementMode(MOVE_None);
		break;
	case EAdvKitMovementMode::Walking:
		SetMovementMode(MOVE_Walking);
		break;
	case EAdvKitMovementMode::NavWalking:
		SetMovementMode(MOVE_NavWalking);
		break;
	case EAdvKitMovementMode::Falling:
		SetMovementMode(MOVE_Falling);
		break;
	case EAdvKitMovementMode::Swimming:
		SetMovementMode(MOVE_Swimming);
		break;
	case EAdvKitMovementMode::Flying:
		SetMovementMode(MOVE_Flying);
		break;
	//Utilize custom movement mode for new modes
	default:
		SetMovementMode(MOVE_Custom, (uint8)NewMovementMode + NewCustomMode);
		break;
	}
}

EAdvKitMovementMode UAdvKitCharacterMovementComponent::GetAdvMovementMode() const
{
	//Convert movement mode to advkit movement mode
	if (MovementMode == MOVE_Custom)
	{
		if (CustomMovementMode >= (uint8)EAdvKitMovementMode::Custom)
		{
			return EAdvKitMovementMode::Custom;
		}

		return (EAdvKitMovementMode)CustomMovementMode;
	}

	return (EAdvKitMovementMode)MovementMode.GetValue();
}

uint8 UAdvKitCharacterMovementComponent::GetAdvCustomMovementMode() const
{
	if (CustomMovementMode >= (uint8)EAdvKitMovementMode::Custom)
	{
		return CustomMovementMode - (uint8)EAdvKitMovementMode::Custom;
	}

	return 0;
}

bool UAdvKitCharacterMovementComponent::IsClimbingWall() const
{
	if (!HasValidData())
	{
		return false;
	}

	return GetAdvMovementMode() == EAdvKitMovementMode::ClimbingWall;
}

bool UAdvKitCharacterMovementComponent::IsClimbingCeiling() const
{
	if (!HasValidData())
	{
		return false;
	}

	return GetAdvMovementMode() == EAdvKitMovementMode::ClimbingCeiling;
}

bool UAdvKitCharacterMovementComponent::IsClimbingLadder() const
{
	if (!HasValidData())
	{
		return false;
	}

	return GetAdvMovementMode() == EAdvKitMovementMode::ClimbingLadder;
}

bool UAdvKitCharacterMovementComponent::IsClimbingLedge() const
{
	if (!HasValidData())
	{
		return false;
	}

	return GetAdvMovementMode() == EAdvKitMovementMode::ClimbingLedge;
}

bool UAdvKitCharacterMovementComponent::IsWalkingTightspace() const
{
	if (!HasValidData())
	{
		return false;
	}

	return GetAdvMovementMode() == EAdvKitMovementMode::WalkingTightspace;
}

bool UAdvKitCharacterMovementComponent::IsWalkingBalance() const
{
	if (!HasValidData())
	{
		return false;
	}

	return GetAdvMovementMode() == EAdvKitMovementMode::WalkingBalance;

}

bool UAdvKitCharacterMovementComponent::LetGoOfZone()
{
	//Cannot let go of zone if not in zone
	auto* LocationInZone = GetZoneLocation();
	if (!LocationInZone)
	{
		return false;
	}

	//Letting go of these zones can only mean falling off
	if (IsClimbingLadder() || IsClimbingWall() || IsClimbingCeiling())
	{
		return AdvKitCharacterOwner->StartTransitionActionTo(EAdvKitMovementMode::Falling, UAdvKitCA_ZoneTransition::MakeArguments((AAdvKitZone*)nullptr));
	}

	//These zones support downwards transitions
	if ((IsWalkingTightspace() || IsClimbingLedge()) && LocationInZone)
	{
		return TryToTransition(
			GetZone(),
			LocationInZone->GetGlobalPosition(),
			-AdvKitCharacterOwner->GetActorUpVector()*JumpZVelocity
			);
	}

	return false;
}

bool UAdvKitCharacterMovementComponent::JumpUpZone()
{
	//switch (GetAdvMovementMode())
	//{
	//case EAdvKitMovementMode::ClimbingWall:
	//	//Nothing done, but this will prevent jumping
	//	return true;
	//case EAdvKitMovementMode::ClimbingLadder:
	//case EAdvKitMovementMode::ClimbingLedge:
	//{
		//Try to transition upwards from zone
		auto* LocationInZone = GetZoneLocation();
		if (!LocationInZone)
		{
			return false;
		}

		FVector TransitionDirection = Acceleration;
		if (TransitionDirection.IsNearlyZero())
		{
			TransitionDirection = CharacterOwner->GetActorUpVector()*JumpZVelocity;
		}

		return TryToTransition(
			GetZone(),
			LocationInZone->GetGlobalPosition(),
			TransitionDirection,
			true
			);
	//}
	//break;
	//}

	//return false;
}

bool UAdvKitCharacterMovementComponent::TryToTransition(AAdvKitZone* ForZone, const FVector& ForWorldPosition, const FVector& InWorldDirection, bool bIsJump)
{
	//Try to find a transition
	UAdvKitTransitionComponent* ClosestTransition = GetPossibleTransition(ForZone, ForWorldPosition, InWorldDirection, bIsJump);
	if (ClosestTransition == nullptr)
	{
		return false;
	}

	//Execute the transition
	return AdvKitCharacterOwner->StartTransitionActionTo(ClosestTransition->TargetPhysics, UAdvKitCA_ZoneTransition::MakeArguments(ClosestTransition));
}

UAdvKitTransitionComponent* UAdvKitCharacterMovementComponent::GetPossibleTransition(AAdvKitZone* ForZone, const FVector& ForWorldPosition, const FVector& InWorldDirection, bool bIsJump)
{
	//Get all transitions for this character
	TArray<UAdvKitTransitionComponent*> AvailableTransitions = ForZone->GetPossibleTransitions(AdvKitCharacterOwner);
	if (AvailableTransitions.Num() == 0)
	{
		return nullptr;
	}

	TArray<UAdvKitTransitionComponent*> PossibleTransitions;

	//Filter out transitions that are traversable with the arguments
	for (auto& TempTransition : AvailableTransitions)
	{
		if(!IsValid(TempTransition))
		{
			continue;
		}

		if (!TempTransition->CanTransition(AdvKitCharacterOwner, ForWorldPosition, InWorldDirection, bIsJump))
		{
			continue;
		}

		PossibleTransitions.Add(TempTransition);
	}

	//Cannot do anything without transitions
	if (PossibleTransitions.Num() == 0)
	{
		return nullptr;
	}

	//Find the closest transition and return it
	float ClosestDistance = 0;
	UAdvKitTransitionComponent* ClosestTransition = nullptr;

	for (auto& TempTransition : PossibleTransitions)
	{
		if (!IsValid(TempTransition))
		{
			continue;
		}

		float Distance = FVector::Dist(ForWorldPosition, TempTransition->GetClosestSourcePosition(ForWorldPosition));
		if (!ClosestTransition || ClosestDistance > Distance)
		{
			ClosestDistance = Distance;
			ClosestTransition = TempTransition;
		}
	}

	return ClosestTransition;
}

bool UAdvKitCharacterMovementComponent::CanTransition(AAdvKitZone* ForZone, const FVector& ForWorldPosition, const FVector& InWorldDirection, bool bIsJump)
{
	//At least one transition can be used
	return GetPossibleTransition(ForZone, ForWorldPosition, InWorldDirection, bIsJump) != nullptr;
}

void UAdvKitCharacterMovementComponent::StartNewPhysics(float DeltaSeconds, int32 Iterations)
{
	if ((DeltaSeconds < 0.0003f) || (Iterations > 7) || !HasValidData())
		return;

	if (UpdatedComponent->IsSimulatingPhysics())
	{
		UE_LOG(LogAdvKitCharacterMovement, Log, TEXT("UCharacterMovementComponent::StartNewPhysics: UpdateComponent (%s) is simulating physics - aborting."), *UpdatedComponent->GetPathName());
		return;
	}

	bMovementInProgress = true;

	//Start new physics depending on mode
	switch (GetAdvMovementMode())
	{
	case EAdvKitMovementMode::None:
		return;
	case EAdvKitMovementMode::Walking:
		PhysWalking(DeltaSeconds, Iterations);
		break;
	case EAdvKitMovementMode::NavWalking:
		PhysNavWalking(DeltaSeconds, Iterations);
		break;
	case EAdvKitMovementMode::Falling:
		PhysFalling(DeltaSeconds, Iterations);
		break;
	case EAdvKitMovementMode::Swimming:
		PhysSwimming(DeltaSeconds, Iterations);
		break;
	case EAdvKitMovementMode::Flying:
		PhysFlying(DeltaSeconds, Iterations);
		break;
	case EAdvKitMovementMode::ClimbingWall:
		PhysClimbWall(DeltaSeconds, Iterations);
		break;
	case EAdvKitMovementMode::ClimbingCeiling:
		PhysClimbCeiling(DeltaSeconds, Iterations);
		break;
	case EAdvKitMovementMode::ClimbingLedge:
		PhysClimbLedge(DeltaSeconds, Iterations);
		break;
	case EAdvKitMovementMode::ClimbingLadder:
		PhysClimbLadder(DeltaSeconds, Iterations);
		break;
	case EAdvKitMovementMode::WalkingTightspace:
		PhysTightSpace(DeltaSeconds, Iterations);
		break;
	case EAdvKitMovementMode::WalkingBalance:
		PhysBalancing(DeltaSeconds, Iterations);
		break;
	case EAdvKitMovementMode::ActionDriven:
		PhysActionDriven(DeltaSeconds, Iterations);
		break;
	case EAdvKitMovementMode::Custom:
	default:
		PhysCustom(DeltaSeconds, Iterations);
		break;
	}

	bMovementInProgress = false;

	if (bDeferUpdateMoveComponent)
	{
		SetUpdatedComponent(DeferredUpdatedMoveComponent);
	}
}

void UAdvKitCharacterMovementComponent::PhysWalking(float DeltaSeconds, int32 Iterations)
{	
	//Test if character is walking over a ledge by first tracing in the walking direction and down
	//to make sure there is enough room beneath the edge. Lastly trace backwards to check if a ledge
	//to grab exists
	FHitResult AbyssHit;
	FVector CapsuleExtent = GetPawnCapsuleExtent(EShrinkCapsuleExtent::SHRINK_None);

	FVector UpVector = UpdatedComponent->GetUpVector();
	FVector ForwardVector = UpdatedComponent->GetForwardVector();

	FVector AbyssStart = UpdatedComponent->GetComponentLocation();
	AbyssStart -= UpVector*CapsuleExtent.Z;
	AbyssStart += UpVector*LedgeSmallHeightOffset;
	AbyssStart += Velocity.GetSafeNormal() * LedgePrecognitionDistance;
	AbyssStart += Velocity*DeltaSeconds;
	
	FVector AbyssEnd = AbyssStart - UpVector*LedgeDownTraceDistance;

	//Check if below is empty
	FCollisionQueryParams Params;
	FCollisionObjectQueryParams ObjectParams;
	if (!GetWorld()->LineTraceSingleByObjectType(AbyssHit, AbyssStart, AbyssEnd, ObjectParams, Params))
	{
		FVector LedgeTraceEnd = UpdatedComponent->GetComponentLocation();
		LedgeTraceEnd -= UpVector*CapsuleExtent.Z;

		//Check if a ledge exists
		FHitResult LedgeHit;
		if (GetWorld()->LineTraceSingleByObjectType(LedgeHit, AbyssEnd, LedgeTraceEnd, ObjectParams, Params))
		{
			auto Zone = Cast<AAdvKitZone>(LedgeHit.GetActor());
			if (Zone)
			{
				//Check if zone is a ledge
				if (Zone->GetPhysics() == EAdvKitMovementMode::ClimbingLedge)
				{
					//Use ledge
					if (AdvKitCharacterOwner->StartTransitionActionTo(Zone->GetPhysics(), UAdvKitCA_ZoneTransition::MakeArguments(Zone)))
					{
						return;
					}
				}
			}
		}
	}

	Super::PhysWalking(DeltaSeconds, Iterations);
}

void UAdvKitCharacterMovementComponent::PhysClimbWall(float DeltaSeconds, int32 Iterations)
{
	FVector ZoneAcceleration = Acceleration.GetClampedToMaxSize(GetMaxSpeed());
	FVector Offset = GetDesiredOffsetFromZone(GetZone(), GetZoneLocation());
	FVector Extent = GetHalfExtentForZone(GetZone());
	PhysMoveInZone(ZoneAcceleration, Offset, DeltaSeconds, Extent, true);
}

void UAdvKitCharacterMovementComponent::PhysClimbCeiling(float DeltaSeconds, int32 Iterations)
{
	auto* LocationInZone = GetZoneLocation();
	FVector ZoneAcceleration = Acceleration.GetClampedToMaxSize(GetMaxSpeed());
	FVector Offset = GetDesiredOffsetFromZone(GetZone(), LocationInZone);
	FVector Extent = GetHalfExtentForZone(GetZone());
	FVector OldLocation = GetActorLocation();
	
	//Move in zone
	if (!PhysMoveInZone(ZoneAcceleration, Offset, DeltaSeconds, Extent, true))
	{
		return;
	}

	if (!LocationInZone)
	{
		return;
	}

	//If the character has not moved as it should have, but still moved it might be because it was constrained by the zone's borders
	FVector DeltaLocation = GetActorLocation() - OldLocation;
	float MovementDot = FVector::DotProduct(DeltaLocation.GetSafeNormal(), ZoneAcceleration.GetSafeNormal());
	if (MovementDot < CeilingMinTransitionDot)
	{
		TryToTransition(
			LocationInZone->Zone,
			LocationInZone->GetGlobalPosition(),
			ZoneAcceleration
			);
	}


}

void UAdvKitCharacterMovementComponent::PhysClimbLedge(float DeltaSeconds, int32 Iterations)
{
	FVector ZoneAcceleration = Acceleration.GetClampedToMaxSize(GetMaxSpeed());
	FVector Offset = GetDesiredOffsetFromZone(GetZone(), GetZoneLocation());
	FVector Extent = GetHalfExtentForZone(GetZone());
	PhysMoveInZone(ZoneAcceleration, Offset, DeltaSeconds, Extent, true);
}

void UAdvKitCharacterMovementComponent::PhysClimbLadder(float DeltaSeconds, int32 Iterations)
{
	FVector ZoneAcceleration = Acceleration.GetClampedToMaxSize(GetMaxSpeed());
	FVector Offset = GetDesiredOffsetFromZone(GetZone(), GetZoneLocation());
	FVector Extent = GetHalfExtentForZone(GetZone());
	PhysMoveInZone(ZoneAcceleration, Offset, DeltaSeconds, Extent, true, false);
}

void UAdvKitCharacterMovementComponent::PhysTightSpace(float DeltaSeconds, int32 Iterations)
{
	FVector ZoneAcceleration = Acceleration.GetClampedToMaxSize(GetMaxSpeed());
	FVector Offset = GetDesiredOffsetFromZone(GetZone(),GetZoneLocation());
	FVector Extent = GetHalfExtentForZone(GetZone());
	PhysMoveInZone(ZoneAcceleration, Offset, DeltaSeconds, Extent, true);
}

void UAdvKitCharacterMovementComponent::PhysBalancing(float DeltaSeconds, int32 Iterations)
{
	if (!(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()))
	{
		//Handle Balance change
		if (!GetWorld()->GetTimerManager().IsTimerActive(ChangeBalanceTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(ChangeBalanceTimerHandle,this, &UAdvKitCharacterMovementComponent::ChangeTargetBalance, BalanceChangeTime, false);
		}

		//Automatic imbalancing
		CurrentBalance += BalanceChangePerSecond*DeltaSeconds;

		//Player trying to re-balance
		float BalanceModificationNormalDot = FVector::DotProduct(UpdatedComponent->GetRightVector(), Acceleration.GetSafeNormal());
		if (FMath::Abs<float>(BalanceModificationNormalDot) > MinCounterBalanceDot)
		{
			CurrentBalance += BalanceModificationNormalDot * DeltaSeconds * CounterBalanceMultiplier;
		}

		//Falling off
		if (CurrentBalance >= MaxBalance || CurrentBalance <= -MaxBalance)
		{
			if (AdvKitCharacterOwner->StartTransitionActionTo(EAdvKitMovementMode::Falling, UAdvKitCA_ZoneTransition::MakeArguments((AAdvKitZone*)nullptr)))
			{
				return;
			}
		}
	}

	FVector ZoneAcceleration = Acceleration.GetClampedToMaxSize(GetMaxSpeed());
	FVector Offset = GetDesiredOffsetFromZone(GetZone(), GetZoneLocation());
	FVector Extent = GetHalfExtentForZone(GetZone());
	PhysMoveInZone(ZoneAcceleration, Offset, DeltaSeconds, Extent, true, true);
}

void UAdvKitCharacterMovementComponent::PhysActionDriven(float DeltaSeconds, int32 Iterations)
{
	if (!AdvKitCharacterOwner)
	{
		return;
	}

	if (!AdvKitCharacterOwner->ActionManager)
	{
		return;
	}

	auto PhysicsAction = Cast<UAdvKitCharacterActionPhysics>(AdvKitCharacterOwner->ActionManager->GetActiveAction());
	if (!PhysicsAction)
	{
		UE_LOG(LogAdvKit,Error, TEXT("UAdvKitCharacterMovementComponent::PhysActionDriven: MovementMode is \"ActionDriven\", but no physics action is active!"));
		return;
	}

	PhysicsAction->ExecutePhysicsMovement(DeltaSeconds);
}

void UAdvKitCharacterMovementComponent::PhysicsRotation(float DeltaSeconds)
{
	if (!HasValidData() || (!CharacterOwner->Controller && !bRunPhysicsWithNoController))
	{
		return;
	}

	bool bInTransition = AdvKitCharacterOwner->GetActiveAction() && AdvKitCharacterOwner->GetActiveAction()->IsA(UAdvKitCA_ZoneTransition::StaticClass());
	if (bInTransition)
	{
		return;
	}

	const FRotator CurrentRotation = UpdatedComponent->GetComponentRotation();
	FRotator DeltaRot = GetDeltaRotation(DeltaSeconds);
	FRotator DesiredRotation = CurrentRotation;

	// Accumulate a desired new rotation.
	FRotator NewRotation = CurrentRotation;
	FVector DeltaMove = FVector::ZeroVector;

	switch (GetAdvMovementMode())
	{
	case EAdvKitMovementMode::None:
	case EAdvKitMovementMode::Walking:
	case EAdvKitMovementMode::NavWalking:
	case EAdvKitMovementMode::Falling:
	case EAdvKitMovementMode::ClimbingCeiling:
	case EAdvKitMovementMode::Swimming:
		Super::PhysicsRotation(DeltaSeconds);
		return;
	//These zones all have fixed orientations for the character
	case EAdvKitMovementMode::ClimbingWall:
	case EAdvKitMovementMode::WalkingTightspace:
	case EAdvKitMovementMode::ClimbingLedge:
	case EAdvKitMovementMode::ClimbingLadder:
	case EAdvKitMovementMode::WalkingBalance:
		NewRotation = GetDesiredRotationInZone(GetZone(), GetZoneLocation());
		break;
	case EAdvKitMovementMode::ActionDriven:
	{
		if (!AdvKitCharacterOwner)
		{
			return;
		}

		if (!AdvKitCharacterOwner->ActionManager)
		{
			return;
		}

		auto PhysicsAction = Cast<UAdvKitCharacterActionPhysics>(AdvKitCharacterOwner->ActionManager->GetActiveAction());
		if (!PhysicsAction)
		{
			UE_LOG(LogAdvKit,Error, TEXT("PhysicsRotation: MovementMode is \"ActionDriven\", but no physics action is active!"));
			return;
		}

		PhysicsAction->ExecutePhysicsRotation(DeltaSeconds);
		return;
	}
	break;
	case EAdvKitMovementMode::Custom:
		break;
	default:
		break;
	}

	// Set the new rotation.
	if (!NewRotation.Equals(CurrentRotation.GetDenormalized(), 0.01f))
	{
		MoveUpdatedComponent(DeltaMove, NewRotation, true);
	}
}

bool UAdvKitCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	if (GetZone())
	{
		return JumpUpZone();
	}

	return Super::DoJump(bReplayingMoves);
}

UAdvKitZoneLocation* UAdvKitCharacterMovementComponent::GetZoneLocation()
{
	if (!AdvKitCharacterOwner)
	{
		return nullptr;
	}

	return AdvKitCharacterOwner->GetZoneLocation();
}


class AAdvKitZone* UAdvKitCharacterMovementComponent::GetZone()
{
	if (!AdvKitCharacterOwner)
	{
		return nullptr;
	}

	return AdvKitCharacterOwner->GetZone();
}

bool UAdvKitCharacterMovementComponent::MoveLocationInZone(FVector GlobalAcceleration, float DeltaSeconds, const FVector& Extent, bool AutoTransition, bool ConstrainAcceleration)
{
	check(AdvKitCharacterOwner);

	//Needs a location to move
	auto* LocationInZone = AdvKitCharacterOwner->GetZoneLocation();
	if (!LocationInZone)
	{
		return false;
	}

	//DrawDebugCoordinateSystem(GetWorld(), LocationInZone->GetGlobalPosition(), UpdatedComponent->GetComponentRotation(), 100);

	FVector ActualAcceleration = ConstrainAcceleration ? LocationInZone->ConstrainDirection(GlobalAcceleration) : GlobalAcceleration;

	//Try to move location in the desired direction
	if (LocationInZone->MoveInDirection(ActualAcceleration*DeltaSeconds, Extent, UpdatedComponent->GetComponentRotation()))
	{
		return true;
	}

	//Location has not moved should a transition be attempted?
	if (!AutoTransition)
	{
		return false;
	}

	//If the character didn't move it is very likely it reached the end of the zone, 
	//it should try to transition when the player still wants to move
	if (GlobalAcceleration.IsNearlyZero())
	{
		return false;
	}

	TryToTransition(
		LocationInZone->Zone,
		LocationInZone->GetGlobalPosition(),
		ActualAcceleration
		);

	//It might transition but character has not moved
	return false;
}

FVector UAdvKitCharacterMovementComponent::GetDesiredOffsetFromZone(AAdvKitZone* Zone, UAdvKitZoneLocation* AtLocation)
{
	if (!AtLocation || !Zone)
	{
		return FVector::ZeroVector;
	}

	EAdvKitMovementMode ForMovementMode = Zone->GetPhysics();
	//uint8 ForCustomMovementMode = Zone->GetCustomPhysics();

	FVector LocalOffset = FVector::ZeroVector;

	switch (ForMovementMode)
	{
	case EAdvKitMovementMode::ClimbingLedge:
		LocalOffset = LedgeOffset;
		break;
	case EAdvKitMovementMode::ClimbingWall:
		LocalOffset = WallOffset;
		break;
	case EAdvKitMovementMode::ClimbingLadder:
		LocalOffset = LadderOffset;
		break;
	case EAdvKitMovementMode::ClimbingCeiling:
		LocalOffset = CeilingOffset;
		break;
	case EAdvKitMovementMode::WalkingTightspace:
		LocalOffset = TightSpaceOffset;
		break;
	case EAdvKitMovementMode::WalkingBalance:
		LocalOffset = BalanceOffset;
		break;
	default:
		break;
	}

	//Transform desired offsets into world space using the location orienation inside the current zone
	FVector OffsetFromZone =
		Zone->GetZoneForwardVector(AtLocation) * LocalOffset.X
		+ Zone->GetZoneRightVector(AtLocation) * LocalOffset.Y
		+ Zone->GetZoneUpVector(AtLocation) * LocalOffset.Z;


	return OffsetFromZone;
}

FRotator UAdvKitCharacterMovementComponent::GetDesiredRotationInZone(AAdvKitZone* Zone, UAdvKitZoneLocation* AtLocation)
{
	FRotator NewRotation = CharacterOwner->GetActorRotation();

	if (!AtLocation || !Zone)
	{
		return NewRotation;
	}

	FVector ZoneForward = Zone->GetZoneForwardVector(AtLocation);
	FVector ZoneRight = Zone->GetZoneRightVector(AtLocation);
	FVector ZoneUp = Zone->GetZoneUpVector(AtLocation);

	EAdvKitMovementMode ForMovementMode = Zone->GetPhysics();
	//uint8 ForCustomMovementMode = Zone->GetCustomPhysics();

	switch (ForMovementMode)
	{
	case EAdvKitMovementMode::WalkingTightspace:
		//Just look at the zone
		NewRotation = ZoneForward.Rotation();
		break;
	case EAdvKitMovementMode::ClimbingWall:
	case EAdvKitMovementMode::ClimbingLedge:
	case EAdvKitMovementMode::ClimbingLadder:
		{
			FQuat ZoneOrientation = FMatrix(
				ZoneForward,
				ZoneRight,
				ZoneUp,
				FVector::ZeroVector
				).Rotator().Quaternion();


			//Look at zone
			FRotator LocalLookAtRotation(0, 180, 0);
			//Convert to global rotation
			NewRotation = (ZoneOrientation * LocalLookAtRotation.Quaternion()).Rotator();
		}
		break;
	case EAdvKitMovementMode::WalkingBalance:
		{
			FVector GlobalForwardVector = CharacterOwner->GetActorForwardVector();
			FVector ZoneForwardVector = ZoneRight;

			//Find out which direction to face
			float DotProduct = FVector::DotProduct(ZoneForwardVector, LastNonZeroAcceleration.GetSafeNormal());
			if (DotProduct > MaxBalanceTurnAroundDot)
			{
				GlobalForwardVector = ZoneForwardVector;
			}
			else if (DotProduct < -MaxBalanceTurnAroundDot)
			{
				GlobalForwardVector = -ZoneForwardVector;
			}
			//Not going forwards or backwards, so the character should align the way it is already facing
			else
			{
				GlobalForwardVector = ZoneForwardVector*FVector::DotProduct(ZoneForwardVector, GlobalForwardVector);
			}

			GlobalForwardVector.Normalize();

			//Convert to global space
			FQuat GlobalDesiredQuat = GlobalForwardVector.Rotation().Quaternion();

			//Calculate delta degrees and lerp alpha
			FQuat CurrentQuat = UpdatedComponent->GetComponentQuat();
			float DegreeDifference = FMath::RadiansToDegrees<float>(FMath::Acos(FVector::DotProduct(GlobalDesiredQuat.GetAxisX(), CurrentQuat.GetAxisX())));
			float Alpha = FMath::FInterpConstantTo(0, DegreeDifference, GetWorld()->DeltaTimeSeconds, BalanceRotationRate) / DegreeDifference;

			//Apply interpolation and set new rotation
			FQuat NewQuat = FQuat::Slerp(UpdatedComponent->GetComponentQuat(), GlobalDesiredQuat, Alpha);
			NewRotation = NewQuat.Rotator();
		}
		break;
	}

	return NewRotation;
}

FVector UAdvKitCharacterMovementComponent::GetHalfExtentForZone(AAdvKitZone* Zone)
{
	//GetPawnCapsuleExtent is not safe to be called on the default object
	auto TempOwner = Cast<AAdvKitCharacter>(GetOwner());
	check(TempOwner);

	float Radius, HalfHeight;
	TempOwner->GetCapsuleComponent()->GetScaledCapsuleSize(Radius, HalfHeight);
	FVector LocalExtent(Radius, Radius, HalfHeight);

	return LocalExtent;
}

FVector UAdvKitCharacterMovementComponent::GetAccelerationInZone()
{
	return Acceleration.GetClampedToMaxSize(GetMaxSpeed());
}


bool UAdvKitCharacterMovementComponent::PhysMoveInZone(FVector GlobalAcceleration, FVector WorldOffset, float DeltaSeconds, const FVector& Extent, bool AutoTransition, bool ConstrainAcceleration)
{
	if ((!AdvKitCharacterOwner || !CharacterOwner->Controller) && !bRunPhysicsWithNoController && !(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()))
	{
		ZeroMovementVariables();
		return false;
	}

	bool bInTransition = AdvKitCharacterOwner->GetActiveAction() && AdvKitCharacterOwner->GetActiveAction()->IsA(UAdvKitCA_ZoneTransition::StaticClass());

	FVector CharacterMovementDelta = FVector::ZeroVector;
	bool bMoved = false;
	if (!(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()) && !bInTransition)
	{
		bMoved = MoveLocationInZone(GlobalAcceleration, DeltaSeconds, Extent, AutoTransition, ConstrainAcceleration);

		auto* LocationInZone = GetZoneLocation();
		if (!IsValid(LocationInZone))
		{
			return false;
		}

		FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
		
		if (DeltaSeconds == 0)
		{
			return false;
		}

		//Velocity is calculated from delta location
		CharacterMovementDelta = LocationInZone->GetGlobalPosition() + WorldOffset - ComponentLocation;
		Velocity = CharacterMovementDelta / DeltaSeconds;
		if (!bMoved && !GlobalAcceleration.IsNearlyZero())
		{
			return false;
		}


		FVector OldLocation = UpdatedComponent->GetComponentLocation();

		FHitResult Hit;
		SafeMoveUpdatedComponent(CharacterMovementDelta, CharacterOwner->GetActorRotation(), true, Hit);
		if (Hit.bBlockingHit && UpdatedComponent)
		{
			SlideAlongSurface(CharacterMovementDelta, 1.f - Hit.Time, Hit.Normal, Hit, true);
		}

		FVector ZoneAdjustDelta = UpdatedComponent->GetComponentLocation() - (OldLocation + CharacterMovementDelta);
		if (!ZoneAdjustDelta.IsNearlyZero())
		{
			bMoved |= MoveLocationInZone(ZoneAdjustDelta, 1.0f, Extent, false, ConstrainAcceleration);
		}
	}
	else
	{
		MoveUpdatedComponent(Velocity*DeltaSeconds, CharacterOwner->GetActorRotation(), false);
	}

	return bMoved;
}

FVector UAdvKitCharacterMovementComponent::GetGlobalLocationInZone()
{
	auto* LocationInZone = GetZoneLocation();
	if (!LocationInZone)
	{
		return FVector::ZeroVector;
	}

	return LocationInZone->GetGlobalPosition();
}

void UAdvKitCharacterMovementComponent::ChangeTargetBalance()
{
	//Random new balance
	BalanceChangePerSecond = FMath::FRandRange(MinBalanceChangePerSecond, MaxBalanceChangePerSecond);
	if (FMath::RandRange(0, 1))
	{
		BalanceChangePerSecond *= -1;
	}
	
	//Restart Timer
	if (IsWalkingBalance())
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(ChangeBalanceTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(ChangeBalanceTimerHandle,this, &UAdvKitCharacterMovementComponent::ChangeTargetBalance, BalanceChangeTime, false);
		}
	}
}



void UAdvKitCharacterMovementComponent::AccumulateRootMotion(FVector MovementDelta, FRotator NewRotation)
{
	if (!CharacterOwner)
	{
		return;
	}

	FTransform RootMotionTransform;

	RootMotionTransform.SetRotation(
		CharacterOwner->GetMesh()->GetComponentQuat().Inverse()
		*
		NewRotation.Quaternion()
		*
		CharacterOwner->GetMesh()->RelativeRotation.Quaternion()
		);

	FVector LocalDelta = CharacterOwner->GetMesh()->GetComponentTransform().InverseTransformVector(MovementDelta);
	RootMotionTransform.SetLocation(LocalDelta);

	RootMotionParams.Accumulate(RootMotionTransform);
}

void UAdvKitCharacterMovementComponent::AccumulateRootMotionRotation(FRotator NewRotation)
{
	FTransform RootMotionTransform;

	RootMotionTransform.SetRotation(
		CharacterOwner->GetMesh()->GetComponentQuat().Inverse()
		*
		NewRotation.Quaternion()
		*
		CharacterOwner->GetMesh()->RelativeRotation.Quaternion()
	);

	RootMotionParams.Accumulate(RootMotionTransform);
}

void UAdvKitCharacterMovementComponent::AddRootMotionMovement(FVector GlobalDelta)
{
	FTransform RootMotionTransform;

	FVector LocalDelta = CharacterOwner->GetMesh()->GetComponentTransform().InverseTransformVector(GlobalDelta);
	RootMotionTransform.SetLocation(LocalDelta);

	RootMotionParams.Accumulate(RootMotionTransform);
}

void UAdvKitCharacterMovementComponent::AddRootMotionRotation(FRotator AdditionalRotation)
{
	FTransform RootMotionTransform;

	RootMotionTransform.SetRotation(
		AdditionalRotation.Quaternion()
		*
		CharacterOwner->GetMesh()->RelativeRotation.Quaternion()
		);

	RootMotionParams.Accumulate(RootMotionTransform);
}
