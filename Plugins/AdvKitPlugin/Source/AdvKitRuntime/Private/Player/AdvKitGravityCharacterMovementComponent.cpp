#include "AdvKitRuntime.h"
#include "Player/AdvKitGravityCharacterMovementComponent.h"
#include "Player/AdvKitOrientationComponent.h"
#include "Player/AdvKitTraceUtilityComponent.h"
#include "Player/AdvKitGravityCharacter.h"

#include "Environment/AdvKitZone.h"
#include "Environment/AdvKitZoneLocation.h"

#include "Environment/AdvKitTransitionComponent.h"


#include "Actions/Movement/AdvKitCA_ZoneTransition.h"

//Begin CharacterMovementComponent.cpp / Wrapper
#include "Navigation/PathFollowingComponent.h"
DEFINE_LOG_CATEGORY_STATIC(LogAdvKitGravityCharacterMovement, Log, All);
//End CharacterMovementComponent.cpp  / Wrapper

//Quick rundown of the wrapped methods:
//Basically everything is like it is in the original movement component, except for a lot 
//of transformations between local and world space. These changes are marked with //Changed



UAdvKitGravityCharacterMovementComponent::UAdvKitGravityCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUpdateWorldOrientationByTracing = false;
	bUpdateWorldOrientationByFloorResult = true;
	bResetOrientationWhenFalling = false;

	bWantsBeginPlay = true;

	NavAgentProps.bCanCrouch = true;
	NavAgentProps.bCanSwim = true;

	bIgnoreBaseRotation = false;	
}

class UAdvKitOrientationComponent* UAdvKitGravityCharacterMovementComponent::GetOrientationComp() const
{
	if (!AdvKitGravityCharacterOwner)
	{
		return NULL;
	}

	return AdvKitGravityCharacterOwner->WorldOrientationComponent;
}

void UAdvKitGravityCharacterMovementComponent::SetGravityVector(const FVector& GravityDirection, const FVector& GravityOrigin, bool bForce)
{
	AdvKitGravityCharacterOwner->SetNewOrientation(-GravityDirection, GravityOrigin, bForce);
}


bool UAdvKitGravityCharacterMovementComponent::CanWallWalkOn(FFindFloorResult NewFloor)
{
	if (!NewFloor.IsWalkableFloor())
	{
		return false;
	}

	if (!AdvKitGravityCharacterOwner)
	{
		return false;
	}

	return AdvKitGravityCharacterOwner->CanWallWalkOn(NewFloor);
}

void UAdvKitGravityCharacterMovementComponent::UpdateWorldOrientation(float DeltaSeconds)
{
	if (!AdvKitGravityCharacterOwner)
	{
		return;
	}

	UAdvKitOrientationComponent* OrientationComp = GetOrientationComp();
	if (!OrientationComp)
	{
		return;
	}

	//Reset if configured and falling
	if (bResetOrientationWhenFalling && IsFalling())
	{
		SetGravityVector(-FVector::UpVector);
	}
	else
	{
		//Set from floor result if configured
		if (bUpdateWorldOrientationByFloorResult && CanWallWalkOn(CurrentFloor))
		{
			SetGravityVector(-CurrentFloor.HitResult.ImpactNormal, CurrentFloor.HitResult.ImpactPoint);
		}

		//Do a trace to update orientation if configured
		if (bUpdateWorldOrientationByTracing)
		{
			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(CharacterOwner);

			FCollisionObjectQueryParams QueryParams;

			if (AdvKitGravityCharacterOwner->TraceWorldOrientation->LineTraceSingle(Hit, Params, QueryParams))
			{
				AActor* HitActor = Hit.Actor.Get();
				UPrimitiveComponent* HitComponent = Hit.Component.Get();
				if (IsWalkable(Hit))
				{
					SetGravityVector(-Hit.ImpactNormal, Hit.ImpactPoint);
				}
			}
		}
	}

}

void UAdvKitGravityCharacterMovementComponent::OnOrientationChanged(const FTransform& OldOrientation)
{
	UAdvKitOrientationComponent* OrientationComp = GetOrientationComp();
	if (!OrientationComp)
	{
		return;
	}

	//Interpolate the new orientation in a way that keeps the previous orientation as well as possible
	FQuat OldWorldRotation = OldOrientation.GetRotation();
	FQuat NewWorldRotation = OrientationComp->GetComponentQuat();
	if (NewWorldRotation != OldWorldRotation)
	{
		//Transform the new world rotation into the old
		FQuat LocalTargetQuat = OldWorldRotation.Inverse() * NewWorldRotation;

		//Make the forward vector of the new rotation planar in order to determine the delta rotation
		FVector LocalTargetVector = LocalTargetQuat.RotateVector(FVector::ForwardVector);
		FVector LocalPlanarVector = LocalTargetVector;
		LocalPlanarVector.Z = 0;
		LocalPlanarVector.Normalize();

		if (LocalPlanarVector.IsNearlyZero())
		{
			return;
		}


		FQuat LocalPlanarQuat = LocalPlanarVector.Rotation().Quaternion();

		//Determine the delta rotation 
		FQuat LocalDeltaQuat = LocalPlanarQuat.Inverse() * LocalTargetQuat;
		LocalDeltaQuat.Normalize();

		//Apply the delta rotation to the current control rotation
		FRotator OldControlRotation = CharacterOwner->GetControlRotation();
		FQuat LocalControlQuat = OldWorldRotation.Inverse() * OldControlRotation.Quaternion();
		LocalControlQuat = LocalDeltaQuat * LocalControlQuat;

		FQuat NewControlQuat = OldWorldRotation * LocalControlQuat;
		FRotator NewControlRotation = NewControlQuat.Rotator();

		//Set the new control rotation
		if (CharacterOwner->GetController())
		{
			CharacterOwner->GetController()->SetControlRotation(NewControlRotation);
		}
	}
}

void UAdvKitGravityCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	auto WorldOrientationComponent = GetOrientationComp();
	if (WorldOrientationComponent)
	{
		WorldOrientationComponent->OnOrientationChanged.AddDynamic(this, &UAdvKitGravityCharacterMovementComponent::OnOrientationChanged);
	}
}

void UAdvKitGravityCharacterMovementComponent::TickComponent(float DeltaSeconds, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (CharacterOwner && CharacterOwner->Role > ROLE_SimulatedProxy)
	{
		//TODO authority check and replication
		if (CharacterOwner->IsLocallyControlled())
		{
			UpdateWorldOrientation(DeltaSeconds);
		}
	}

	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);
}

void UAdvKitGravityCharacterMovementComponent::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	if (NewUpdatedComponent)
	{
		const AAdvKitGravityCharacter* NewCharacterOwner = Cast<AAdvKitGravityCharacter>(NewUpdatedComponent->GetOwner());
		if (NewCharacterOwner == NULL)
		{
			UE_LOG(LogAdvKitGravityCharacterMovement, Error, TEXT("%s owned by %s must update a component owned by an AdvKitGravityCharacter"), *GetName(), *GetNameSafe(NewUpdatedComponent->GetOwner()));
			return;
		}

	}

	Super::SetUpdatedComponent(NewUpdatedComponent);

	auto NewCharacterOwner = Cast<AAdvKitGravityCharacter>(PawnOwner);
	if (NewCharacterOwner)
	{
		AdvKitGravityCharacterOwner = NewCharacterOwner;
	}
}

void UAdvKitGravityCharacterMovementComponent::TransformMovementVariablesWorldToLocal()
{
	//Variables are already local space
	if (bMovementVariablesAreLocal)
	{
		return;
	}

	auto Orientation = GetOrientationComp();
	if (!Orientation)
	{
		return;
	}

	//Transform and set flag
	Acceleration = Orientation->TransformDirectionW2L(Acceleration);
	Velocity = Orientation->TransformDirectionW2L(Velocity);
	bMovementVariablesAreLocal = true;
}

void UAdvKitGravityCharacterMovementComponent::TransformMovementVariablesLocalToWorld()
{
	//Variables are already world space
	if (!bMovementVariablesAreLocal)
	{
		return;
	}

	auto Orientation = GetOrientationComp();
	if (!Orientation)
	{
		return;
	}

	//Transform and set flag
	Acceleration = GetOrientationComp()->TransformDirectionL2W(Acceleration);
	Velocity = GetOrientationComp()->TransformDirectionL2W(Velocity);
	bMovementVariablesAreLocal = false;
}

FVector UAdvKitGravityCharacterMovementComponent::ConstrainDirectionToPlane(FVector Direction) const
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		return Super::ConstrainDirectionToPlane(Direction);
	}

	//Constrain to gravity altered plane
	if (bConstrainToPlane)
	{
		Direction = Direction - GetOrientationComp()->TransformDirectionL2W(PlaneConstraintNormal * (GetOrientationComp()->TransformDirectionW2L(Direction) | PlaneConstraintNormal));
	}

	return Direction;
}

void UAdvKitGravityCharacterMovementComponent::SetFromSweepHACK(FFindFloorResult& Out, const FHitResult& InHit, const float InSweepFloorDist, const bool bIsWalkableFloor)
{
	Out.bBlockingHit = InHit.IsValidBlockingHit();
	Out.bWalkableFloor = bIsWalkableFloor;
	Out.bLineTrace = false;
	Out.FloorDist = InSweepFloorDist;
	Out.LineDist = 0.f;
	Out.HitResult = InHit;
}

void UAdvKitGravityCharacterMovementComponent::SetFromLineTraceHACK(FFindFloorResult& Out, const FHitResult& InHit, const float InSweepFloorDist, const float InLineDist, const bool bIsWalkableFloor)
{
	// We require a sweep that hit if we are going to use a line result.
	check(Out.HitResult.bBlockingHit);
	if (Out.HitResult.bBlockingHit && InHit.bBlockingHit)
	{
		// override most of the sweep result with the line result, but save some values
		FHitResult OldHit(Out.HitResult);
		Out.HitResult = InHit;

		// Restore some of the old values. We want the new normals and hit actor, however.
		Out.HitResult.Time = OldHit.Time;
		Out.HitResult.ImpactPoint = OldHit.ImpactPoint;
		Out.HitResult.Location = OldHit.Location;
		Out.HitResult.TraceStart = OldHit.TraceStart;
		Out.HitResult.TraceEnd = OldHit.TraceEnd;

		Out.bLineTrace = true;
		Out.LineDist = InLineDist;
		Out.bWalkableFloor = bIsWalkableFloor;
	}
}

bool UAdvKitGravityCharacterMovementComponent::IsWithinEdgeTolerance(const FVector& CapsuleLocation, const FVector& TestImpactPoint, const float CapsuleRadius) const
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		return Super::IsWithinEdgeTolerance(CapsuleLocation, TestImpactPoint, CapsuleRadius);
	}

	const float DistFromCenterSq = GetOrientationComp()->TransformDirectionW2L(TestImpactPoint - CapsuleLocation).SizeSquared2D(); //Changed
	const float ReducedRadiusSq = FMath::Square(FMath::Max(KINDA_SMALL_NUMBER, CapsuleRadius - SWEEP_EDGE_REJECT_DISTANCE));
	return DistFromCenterSq < ReducedRadiusSq;
}

void UAdvKitGravityCharacterMovementComponent::ComputeFloorDist(const FVector& CapsuleLocation, float LineDistance, float SweepDistance, FFindFloorResult& OutFloorResult, float SweepRadius, const FHitResult* DownwardSweepResult) const
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		Super::ComputeFloorDist(CapsuleLocation, LineDistance, SweepDistance, OutFloorResult, SweepRadius, DownwardSweepResult);
		return;
	}

	OutFloorResult.Clear();

	// No collision, no floor...
	if (!UpdatedComponent->IsCollisionEnabled())
	{
		return;
	}

	float PawnRadius, PawnHalfHeight;
	CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

	bool bSkipSweep = false;
	if (DownwardSweepResult != NULL && DownwardSweepResult->IsValidBlockingHit())
	{
		// Only if the supplied sweep was vertical and downward.
		FVector TraceDiffLocal = GetOrientationComp()->TransformDirectionW2L(DownwardSweepResult->TraceStart - DownwardSweepResult->TraceEnd); //Changed
		if (TraceDiffLocal.Z > 0 && TraceDiffLocal.SizeSquared2D() <= KINDA_SMALL_NUMBER) //Changed
		{
			// Reject hits that are barely on the cusp of the radius of the capsule
			if (IsWithinEdgeTolerance(DownwardSweepResult->Location, DownwardSweepResult->ImpactPoint, PawnRadius))
			{
				// Don't try a redundant sweep, regardless of whether this sweep is usable.
				bSkipSweep = true;

				const bool bIsWalkable = IsWalkable(*DownwardSweepResult);
				const float FloorDist = GetOrientationComp()->TransformDirectionW2L(CapsuleLocation - DownwardSweepResult->Location).Z; //Changed
				
				//This is necessary because there is no way to set OutFloorResult directly
				UAdvKitGravityCharacterMovementComponent* MutableThis = const_cast<UAdvKitGravityCharacterMovementComponent*>(this); //Changed
				MutableThis->SetFromSweepHACK(OutFloorResult, *DownwardSweepResult, FloorDist, bIsWalkable); //Changed
				//OutFloorResult.SetFromSweep(*DownwardSweepResult, FloorDist, bIsWalkable); //Original

				if (bIsWalkable)
				{
					// Use the supplied downward sweep as the floor hit result.			
					return;
				}
			}
		}
	}

	// We require the sweep distance to be >= the line distance, otherwise the HitResult can't be interpreted as the sweep result.
	if (SweepDistance < LineDistance)
	{
		check(SweepDistance >= LineDistance);
		return;
	}

	bool bBlockingHit = false;
	FCollisionQueryParams QueryParams(NAME_None, false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(QueryParams, ResponseParam);
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

	// Sweep test
	if (!bSkipSweep && SweepDistance > 0.f && SweepRadius > 0.f)
	{
		// Use a shorter height to avoid sweeps giving weird results if we start on a surface.
		// This also allows us to adjust out of penetrations.
		const float ShrinkScale = 0.9f;
		const float ShrinkScaleOverlap = 0.6f;
		float ShrinkHeight = (PawnHalfHeight - PawnRadius) * (1.f - ShrinkScale);
		float TraceDist = SweepDistance + ShrinkHeight;

		static const FName ComputeFloorDistName(TEXT("ComputeFloorDistSweep"));
		QueryParams.TraceTag = ComputeFloorDistName;
		FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(SweepRadius, PawnHalfHeight - ShrinkHeight);

		FHitResult Hit(1.f);
		if (bUseFlatBaseForFloorChecks)
		{
			const FCollisionShape BoxShape = FCollisionShape::MakeBox(FVector(CapsuleShape.GetCapsuleRadius(), CapsuleShape.GetCapsuleRadius(), CapsuleShape.GetCapsuleHalfHeight()));
			bBlockingHit = GetWorld()->SweepSingleByChannel(Hit, CapsuleLocation, CapsuleLocation + GetOrientationComp()->GetUpVector()*(-TraceDist), GetOrientationComp()->GetComponentQuat(), CollisionChannel, BoxShape, QueryParams, ResponseParam); //Changed
		}
		else
		{
			bBlockingHit = GetWorld()->SweepSingleByChannel(Hit, CapsuleLocation, CapsuleLocation + GetOrientationComp()->GetUpVector()*(-TraceDist), GetOrientationComp()->GetComponentQuat(), CollisionChannel, CapsuleShape, QueryParams, ResponseParam); //Changed
		}

		if (bBlockingHit)
		{
			// Reject hits adjacent to us, we only care about hits on the bottom portion of our capsule.
			// Check 2D distance to impact point, reject if within a tolerance from radius.
			if (Hit.bStartPenetrating || !IsWithinEdgeTolerance(CapsuleLocation, Hit.ImpactPoint, CapsuleShape.Capsule.Radius))
			{
				// Use a capsule with a slightly smaller radius and shorter height to avoid the adjacent object.
				ShrinkHeight = (PawnHalfHeight - PawnRadius) * (1.f - ShrinkScaleOverlap);
				TraceDist = SweepDistance + ShrinkHeight;
				CapsuleShape.Capsule.Radius = FMath::Max(0.f, CapsuleShape.Capsule.Radius - SWEEP_EDGE_REJECT_DISTANCE - KINDA_SMALL_NUMBER);
				CapsuleShape.Capsule.HalfHeight = FMath::Max(PawnHalfHeight - ShrinkHeight, 0.1f);
				Hit.Reset(1.f, false);

				if (bUseFlatBaseForFloorChecks)
				{
					const FCollisionShape BoxShape = FCollisionShape::MakeBox(FVector(CapsuleShape.GetCapsuleRadius(), CapsuleShape.GetCapsuleRadius(), CapsuleShape.GetCapsuleHalfHeight()));
					bBlockingHit = GetWorld()->SweepSingleByChannel(Hit, CapsuleLocation, CapsuleLocation + GetOrientationComp()->GetUpVector()*(-TraceDist), GetOrientationComp()->GetComponentQuat(), CollisionChannel, BoxShape, QueryParams, ResponseParam);
				}
				else
				{
					bBlockingHit = GetWorld()->SweepSingleByChannel(Hit, CapsuleLocation, CapsuleLocation + GetOrientationComp()->GetUpVector()*(-TraceDist), GetOrientationComp()->GetComponentQuat(), CollisionChannel, CapsuleShape, QueryParams, ResponseParam);
				}
			}

			// Reduce hit distance by ShrinkHeight because we shrank the capsule for the trace.
			// We allow negative distances here, because this allows us to pull out of penetrations.
			const float MaxPenetrationAdjust = FMath::Max(MAX_FLOOR_DIST, PawnRadius);
			const float SweepResult = FMath::Max(-MaxPenetrationAdjust, Hit.Time * TraceDist - ShrinkHeight);

			//This is necessary because there is no way to set OutFloorResult directly
			UAdvKitGravityCharacterMovementComponent* MutableThis = const_cast<UAdvKitGravityCharacterMovementComponent*>(this); //Changed
			MutableThis->SetFromSweepHACK(OutFloorResult, Hit, SweepResult, false); //Changed
			//OutFloorResult.SetFromSweep(Hit, SweepResult, false); //Original
			if (Hit.IsValidBlockingHit() && IsWalkable(Hit))
			{
				if (SweepResult <= SweepDistance)
				{
					// Hit within test distance.
					OutFloorResult.bWalkableFloor = true;
					return;
				}
			}
		}
	}

	// Since we require a longer sweep than line trace, we don't want to run the line trace if the sweep missed everything.
	// We do however want to try a line trace if the sweep was stuck in penetration.
	if (!OutFloorResult.bBlockingHit && !OutFloorResult.HitResult.bStartPenetrating)
	{
		OutFloorResult.FloorDist = SweepDistance;
		return;
	}

	// Line trace
	if (LineDistance > 0.f)
	{
		const float ShrinkHeight = PawnHalfHeight;
		const FVector LineTraceStart = CapsuleLocation;
		const float TraceDist = LineDistance + ShrinkHeight;
		const FVector Down = -GetOrientationComp()->GetUpVector()*TraceDist; //Changed

		static const FName FloorLineTraceName = FName(TEXT("ComputeFloorDistLineTrace"));
		QueryParams.TraceTag = FloorLineTraceName;

		FHitResult Hit(1.f);
		bBlockingHit = GetWorld()->LineTraceSingleByChannel(Hit, LineTraceStart, LineTraceStart + Down, CollisionChannel, QueryParams, ResponseParam);

		if (bBlockingHit)
		{
			if (Hit.Time > 0.f)
			{
				// Reduce hit distance by ShrinkHeight because we started the trace higher than the base.
				// We allow negative distances here, because this allows us to pull out of penetrations.
				const float MaxPenetrationAdjust = FMath::Max(MAX_FLOOR_DIST, PawnRadius);
				const float LineResult = FMath::Max(-MaxPenetrationAdjust, Hit.Time * TraceDist - ShrinkHeight);

				OutFloorResult.bBlockingHit = true;
				if (LineResult <= LineDistance && IsWalkable(Hit))
				{
					//This is necessary because there is no way to set OutFloorResult directly
					UAdvKitGravityCharacterMovementComponent* MutableThis = const_cast<UAdvKitGravityCharacterMovementComponent*>(this); //Changed
					MutableThis->SetFromLineTraceHACK(OutFloorResult, Hit, OutFloorResult.FloorDist, LineResult, true); //Changed
					//OutFloorResult.SetFromLineTrace(Hit, OutFloorResult.FloorDist, LineResult, true); //Original
					return;
				}
			}
		}
	}

	// No hits were acceptable.
	OutFloorResult.bWalkableFloor = false;
	OutFloorResult.FloorDist = SweepDistance;
}

void UAdvKitGravityCharacterMovementComponent::TwoWallAdjust(FVector &Delta, const FHitResult& Hit, const FVector &OldHitNormal) const
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		Super::TwoWallAdjust(Delta, Hit, OldHitNormal);
		return;
	}

	Super::Super::TwoWallAdjust(Delta, Hit, OldHitNormal); //Changed, Skip original
	FVector InDelta = GetOrientationComp()->TransformDirectionW2L(Delta); //Changed
	Delta = GetOrientationComp()->TransformDirectionW2L(Delta); //Changed
	FVector LocalHitNormal = GetOrientationComp()->TransformDirectionW2L(Hit.Normal); //Changed

	if (IsMovingOnGround())
	{
		// Allow slides up walkable surfaces, but not unwalkable ones (treat those as vertical barriers).
		if (Delta.Z > 0.f)
		{
			if ((LocalHitNormal.Z >= GetWalkableFloorZ() || IsWalkable(Hit)) && LocalHitNormal.Z > KINDA_SMALL_NUMBER)
			{
				// Maintain horizontal velocity
				const float Time = (1.f - Hit.Time);
				const FVector ScaledDelta = Delta.GetSafeNormal() * InDelta.Size(); //Changed
				Delta = FVector(InDelta.X, InDelta.Y, ScaledDelta.Z / LocalHitNormal.Z) * Time; //Changed
			}
			else
			{
				Delta.Z = 0.f;
			}
		}
		else if (Delta.Z < 0.f)
		{
			// Don't push down into the floor.
			if (CurrentFloor.FloorDist < MIN_FLOOR_DIST && CurrentFloor.bBlockingHit)
			{
				Delta.Z = 0.f;
			}
		}
	}

	Delta = GetOrientationComp()->TransformDirectionL2W(Delta);
}

FVector UAdvKitGravityCharacterMovementComponent::HandleSlopeBoosting(const FVector& SlideResult, const FVector& Delta, const float Time, const FVector& Normal, const FHitResult& Hit) const
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		return Super::HandleSlopeBoosting(SlideResult, Delta, Time, Normal, Hit);
	}

	FVector Result = GetOrientationComp()->TransformDirectionW2L(SlideResult); //Changed

	if (Result.Z > 0.f)
	{
		// Don't move any higher than we originally intended.
		const float ZLimit = GetOrientationComp()->TransformDirectionW2L(Delta).Z * Time; //Changed
		if (Result.Z - ZLimit > KINDA_SMALL_NUMBER)
		{
			if (ZLimit > 0.f)
			{
				// Rescale the entire vector (not just the Z component) otherwise we change the direction and likely head right back into the impact.
				const float UpPercent = ZLimit / Result.Z;
				Result *= UpPercent;
			}
			else
			{
				// We were heading down but were going to deflect upwards. Just make the deflection horizontal.
				Result = FVector::ZeroVector;
			}

			// Make remaining portion of original result horizontal and parallel to impact normal.
			const FVector RemainderXY = (SlideResult - Result) * FVector(1.f, 1.f, 0.f);
			const FVector NormalXY = Normal.GetSafeNormal2D();
			const FVector Adjust = Super::Super::ComputeSlideVector(RemainderXY, 1.f, NormalXY, Hit); //Changed
			Result += Adjust;
		}
	}

	return GetOrientationComp()->TransformDirectionL2W(Result); //Changed
}

bool UAdvKitGravityCharacterMovementComponent::IsValidLandingSpot(const FVector& CapsuleLocation, const FHitResult& Hit) const
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		return Super::IsValidLandingSpot(CapsuleLocation, Hit);
	}

	if (!Hit.bBlockingHit)
	{
		return false;
	}

	// Reject unwalkable floor normals.
	if (!IsWalkable(Hit))
	{
		return false;
	}

	// Skip some checks if penetrating. Penetration will be handled by the FindFloor call (using a smaller capsule)
	if (!Hit.bStartPenetrating)
	{
		float PawnRadius, PawnHalfHeight;
		CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

		// Reject hits that are above our lower hemisphere (can happen when sliding down a vertical surface).
		const float LowerHemisphereZ = GetOrientationComp()->TransformPositionW2L(Hit.Location).Z - PawnHalfHeight + PawnRadius; //Changed
		if (GetOrientationComp()->TransformPositionW2L(Hit.ImpactPoint).Z >= LowerHemisphereZ) //Changed
		{
			return false;
		}

		// Reject hits that are barely on the cusp of the radius of the capsule
		if (!IsWithinEdgeTolerance(Hit.Location, Hit.ImpactPoint, PawnRadius))
		{
			return false;
		}
	}

	FFindFloorResult FloorResult;
	FindFloor(CapsuleLocation, FloorResult, false, &Hit);

	if (!FloorResult.IsWalkableFloor())
	{
		return false;
	}

	return true;
}

void UAdvKitGravityCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
		return;
	}

	if (!HasValidData())
	{
		return;
	}

	// React to changes in the movement mode.
	EAdvKitMovementMode CurrentMode = GetAdvMovementMode();
	if (CurrentMode == EAdvKitMovementMode::Walking)
	{
		// Walking uses only XY velocity, and must be on a walkable floor, with a Base.
		Velocity = GetOrientationComp()->TransformDirectionW2L(Velocity);
		Velocity.Z = 0;
		Velocity = GetOrientationComp()->TransformDirectionL2W(Velocity);

		bCrouchMaintainsBaseLocation = true;

		FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, false);
		AdjustFloorHeight();
		SetBase(CurrentFloor.HitResult.Component.Get());

		// make sure we validate our new floor/base on initial entry of the walking physics, even if velocity is zero.
		bForceNextFloorCheck = true;
	}
	else
	{
		CurrentFloor.Clear();
		bCrouchMaintainsBaseLocation = false;

		if (CurrentMode == EAdvKitMovementMode::Falling)
		{
			Velocity += GetImpartedMovementBaseVelocity();
			CharacterOwner->Falling();
		}

		SetBase(NULL);

		if (CurrentMode == EAdvKitMovementMode::None)
		{
			// Kill velocity and clear queued up events
			StopMovementImmediately();
			CharacterOwner->ClearJumpInput();
		}
	}

	CharacterOwner->OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UAdvKitGravityCharacterMovementComponent::MaintainHorizontalGroundVelocity()
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		Super::MaintainHorizontalGroundVelocity();
		return;
	}

	bool bWasLocalInitially = true; //Changed
	if (!bMovementVariablesAreLocal) //Changed
	{
		TransformMovementVariablesWorldToLocal(); 
		bWasLocalInitially = false;
	}

	if (Velocity.Z != 0.f)
	{
		if (bMaintainHorizontalGroundVelocity)
		{
			// Ramp movement already maintained the velocity, so we just want to remove the vertical component.
			Velocity.Z = 0.f;
		}
		else
		{
			// Rescale velocity to be horizontal but maintain magnitude of last update.
			Velocity = Velocity.GetSafeNormal2D() * Velocity.Size();
		}
	}

	if (!bWasLocalInitially)
	{
		TransformMovementVariablesLocalToWorld();
	}
}

void UAdvKitGravityCharacterMovementComponent::PhysWalking(float DeltaSeconds, int32 Iterations)
{
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

	FCollisionQueryParams Params;
	FCollisionObjectQueryParams ObjectParams;
	if (!GetWorld()->LineTraceSingleByObjectType(AbyssHit, AbyssStart, AbyssEnd, ObjectParams, Params))
	{
	//	DrawDebugLine(GetWorld(), AbyssStart, AbyssEnd, FColor::Red);
	//}
	//else
	//{
	//	DrawDebugLine(GetWorld(), AbyssStart, AbyssEnd, FColor::Green);

		FVector LedgeTraceEnd = UpdatedComponent->GetComponentLocation();
		LedgeTraceEnd -= UpVector*CapsuleExtent.Z;


		FHitResult LedgeHit;
		if (GetWorld()->LineTraceSingleByObjectType(LedgeHit, AbyssEnd, LedgeTraceEnd, ObjectParams, Params))
		{
			auto Zone = Cast<AAdvKitZone>(LedgeHit.GetActor());
			if (Zone)
			{
				bool bIsLedge = Zone->GetPhysics() == EAdvKitMovementMode::ClimbingLedge;
				if (bIsLedge)
				{
					//DrawDebugLine(GetWorld(), AbyssEnd, LedgeTraceEnd, FColor::Yellow);
					if (AdvKitGravityCharacterOwner->StartTransitionActionTo(Zone->GetPhysics(), UAdvKitCA_ZoneTransition::MakeArguments(Zone)))
					{
						return;
					}
				}
			}
		}
	}

	if (!GetOrientationComp())
	{
		Super::PhysWalking(DeltaSeconds, Iterations);
		return;
	}

	if ((!AdvKitGravityCharacterOwner || !CharacterOwner->Controller) && !bRunPhysicsWithNoController && !(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()))
	{
		ZeroMovementVariables();
		return;
	}

	if (!UpdatedComponent->IsCollisionEnabled())
	{
		SetAdvMovementMode(EAdvKitMovementMode::Walking);
		return;
	}

	TransformMovementVariablesWorldToLocal(); //Changed

	// Ensure velocity is horizontal.
	MaintainHorizontalGroundVelocity();

	//bound acceleration
	Acceleration.Z = 0.f;
	if (!(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()))
	{
		CalcVelocity(DeltaSeconds, GroundFriction, false, BrakingDecelerationWalking);
	}

	FVector DesiredMove = Velocity;
	DesiredMove.Z = 0.f;

	//Perform the move
	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FFindFloorResult OldFloor = CurrentFloor;
	UPrimitiveComponent* const OldBase = CharacterOwner->GetMovementBase();
	const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
	bJustTeleported = false;
	bool bCheckedFall = false;
	float remainingTime = DeltaSeconds;

	while ((remainingTime > 0.f) && (Iterations < 8) && (CharacterOwner->Controller || bRunPhysicsWithNoController || (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())))
	{
		Iterations++;
		// subdivide moves to be no longer than 0.05 seconds
		const float timeTick = (remainingTime > 0.05f) ? FMath::Min(0.05f, remainingTime * 0.5f) : remainingTime;
		remainingTime -= timeTick;
		const FVector Delta = timeTick * DesiredMove;
		const FVector subLoc = CharacterOwner->GetActorLocation();
		const bool bZeroDelta = Delta.IsNearlyZero();
		FStepDownResult StepDownResult;

		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			// @todo hunting down NaN TTP 304692
			checkf(!Delta.ContainsNaN(), TEXT("PhysWalking: NewTransform contains NaN (%s: %s)\n%s"), *GetPathNameSafe(this), *GetPathNameSafe(GetOuter()), *Delta.ToString());
			// @todo hunting down NaN TTP 304692

			// try to move forward
			MoveAlongFloor(GetOrientationComp()->TransformDirectionL2W(DesiredMove), timeTick, &StepDownResult); //Changed

			if (IsFalling())
			{
				// pawn decided to jump up
				const float ActualDist = GetOrientationComp()->TransformDirectionW2L(CharacterOwner->GetActorLocation() - subLoc).Size2D(); //Changed
				const float DesiredDist = Delta.Size();
				remainingTime += timeTick * (1 - FMath::Min(1.f, ActualDist / DesiredDist));

				TransformMovementVariablesLocalToWorld(); //Changed
				StartNewPhysics(remainingTime, Iterations);
				return;
			}
			else if (IsSwimming()) //just entered water
			{
				TransformMovementVariablesLocalToWorld(); //Changed
				StartSwimmingHACK(OldLocation, Velocity, timeTick, remainingTime, Iterations); //Changed
				return;
			}
		}
		// Update floor.
		// StepUp might have already done it for us.
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
		}

		// check for ledges here
		const bool bCheckLedges = !CanWalkOffLedges();
		if (bCheckLedges && !CurrentFloor.IsWalkableFloor())
		{
			// calculate possible alternate movement
			const FVector GravDir = -GetOrientationComp()->GetUpVector();//Changed
			FVector NewDelta = GetOrientationComp()->TransformDirectionW2L(GetLedgeMove(OldLocation, GetOrientationComp()->TransformDirectionL2W(Delta), GravDir)); //Changed
			if (!NewDelta.IsZero())
			{
				// first revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				// @todo hunting down NaN TTP 304692
				check(timeTick != 0.f);
				// redo move using NewDelta
				DesiredMove = NewDelta / timeTick;
				remainingTime += timeTick;
			}
			else
			{
				// see if it is OK to jump
				// @todo collision : only thing that can be problem is that oldbase has world collision on
				bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, GetOrientationComp()->TransformDirectionL2W(Delta), subLoc, remainingTime, timeTick, Iterations, bMustJump))
				{
					TransformMovementVariablesLocalToWorld(); //Changed
					return;
				}
				bCheckedFall = true;

				// revert this move
				//UE_LOG(LogCharacterMovement, Log, TEXT("%s REVERT MOVE 1"), *CharacterOwner->GetName());
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
			}
		}
		else
		{
			// Validate the floor check
			if (CurrentFloor.IsWalkableFloor())
			{
				if (ShouldCatchAir(OldFloor, CurrentFloor))
				{
					TransformMovementVariablesLocalToWorld(); //Changed
					CharacterOwner->OnWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal, OldLocation, timeTick);
					if (IsMovingOnGround())
					{
						// If still walking, then fall. If not, assume the user set a different mode they want to keep.
						StartFalling(Iterations, remainingTime, timeTick, GetOrientationComp()->TransformDirectionL2W(Delta), subLoc);
					}
					return;
				}

				AdjustFloorHeight();
				SetBase(CurrentFloor.HitResult.Component.Get());
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				// The floor check failed because it started in penetration
				// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + GetOrientationComp()->TransformDirectionL2W(FVector(0.f, 0.f, MAX_FLOOR_DIST)); //Changed
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, CharacterOwner->GetActorRotation());
			}

			// check if just entered water
			if (IsSwimming())
			{
				TransformMovementVariablesLocalToWorld(); //Changed
				StartSwimmingHACK(OldLocation, Velocity, timeTick, remainingTime, Iterations); //Changed
				return;
			}

			// See if we need to start falling.
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, GetOrientationComp()->TransformDirectionL2W(Delta), subLoc, remainingTime, timeTick, Iterations, bMustJump))
				{
					TransformMovementVariablesLocalToWorld(); //Changed
					return;
				}
				bCheckedFall = true;
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (CharacterOwner->GetActorLocation() == subLoc)
		{
			break;
		}
	}

	// Allow overlap events and such to change physics state and velocity
	if (IsMovingOnGround())
	{
		// Make velocity reflect actual move
		if (!bJustTeleported && !(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()))
		{
			Velocity = GetOrientationComp()->TransformDirectionW2L(CharacterOwner->GetActorLocation() - OldLocation) / DeltaSeconds; //Changed
		}

		MaintainHorizontalGroundVelocity();
	}

	TransformMovementVariablesLocalToWorld(); //Changed
}

void UAdvKitGravityCharacterMovementComponent::PhysFlying(float DeltaSeconds, int32 Iterations)
{
	Super::PhysFlying(DeltaSeconds, Iterations);
}

void UAdvKitGravityCharacterMovementComponent::UpdateFloorFromAdjustment()
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		Super::UpdateFloorFromAdjustment();
		return;
	}

	if (!HasValidData())
	{
		return;
	}

	// If walking, try to update the cached floor so it is current. This is necessary for UpdateBasedMovement() and MoveAlongFloor() to work properly.
	// If base is now NULL, presumably we are no longer walking. If we had a valid floor but don't find one now, we'll likely start falling.
	if (CharacterOwner->GetMovementBase() && GetOrientationComp()->TransformDirectionW2L(Velocity).Z <= 0.f)
	{
		FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, false);
	}
	else
	{
		CurrentFloor.Clear();
	}

	bForceNextFloorCheck = true;
}

void UAdvKitGravityCharacterMovementComponent::SimulateMovement(float DeltaSeconds)
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		Super::SimulateMovement(DeltaSeconds);
		return;
	}

	if (!HasValidData() || UpdatedComponent->IsSimulatingPhysics())
	{
		return;
	}

	const bool bIsSimulatedProxy = (CharacterOwner->Role == ROLE_SimulatedProxy);

	// Workaround for replication not being updated initially
	if (bIsSimulatedProxy &&
		CharacterOwner->ReplicatedMovement.Location.IsZero() &&
		CharacterOwner->ReplicatedMovement.Rotation.IsZero() &&
		CharacterOwner->ReplicatedMovement.LinearVelocity.IsZero())
	{
		return;
	}

	// If base is not resolved on the client, we should not try to simulate at all
	if (CharacterOwner->GetBasedMovement().IsBaseUnresolved())
	{
		UE_LOG(LogAdvKitGravityCharacterMovement, Verbose, TEXT("Base for simulated character '%s' is not resolved on client, skipping SimulateMovement"), *CharacterOwner->GetName());
		return;
	}

	FVector OldVelocity;
	FVector OldLocation;

	// Scoped updates can improve performance of multiple MoveComponent calls.
	{
		FScopedMovementUpdate ScopedMovementUpdate(UpdatedComponent, bEnableScopedMovementUpdates ? EScopedUpdate::DeferredUpdates : EScopedUpdate::ImmediateUpdates);

		if (bIsSimulatedProxy)
		{
			// Handle network changes
			if (bNetworkUpdateReceived)
			{
				bNetworkUpdateReceived = false;
				if (bNetworkMovementModeChanged)
				{
					bNetworkMovementModeChanged = false;
					ApplyNetworkMovementMode(CharacterOwner->GetReplicatedMovementMode());
				}
				else if (bJustTeleported)
				{
					// Make sure floor is current. We will continue using the replicated base, if there was one.
					bJustTeleported = false;
					UpdateFloorFromAdjustment();
				}
			}

			HandlePendingLaunch();
		}

		if (MovementMode == MOVE_None)
		{
			return;
		}

		Acceleration = Velocity.GetSafeNormal();	// Not currently used for simulated movement
		AnalogInputModifier = 1.0f;				// Not currently used for simulated movement

		MaybeUpdateBasedMovement(DeltaSeconds);

		// simulated pawns predict location
		OldVelocity = Velocity;
		OldLocation = UpdatedComponent->GetComponentLocation();
		FStepDownResult StepDownResult;
		MoveSmooth(Velocity, DeltaSeconds, &StepDownResult);

		// if simulated gravity, find floor and check if falling
		const bool bEnableFloorCheck = (!CharacterOwner->bSimGravityDisabled || !bIsSimulatedProxy);
		if (bEnableFloorCheck && (MovementMode == MOVE_Walking || MovementMode == MOVE_Falling))
		{
			const FVector CollisionCenter = UpdatedComponent->GetComponentLocation();
			if (StepDownResult.bComputedFloor)
			{
				CurrentFloor = StepDownResult.FloorResult;
			}
			else if (GetOrientationComp()->TransformDirectionW2L(Velocity).Z <= 0.f) //Changed
			{
				FindFloor(CollisionCenter, CurrentFloor, Velocity.IsZero(), NULL);
			}
			else
			{
				CurrentFloor.Clear();
			}

			if (!CurrentFloor.IsWalkableFloor())
			{
				// No floor, must fall.
				TransformMovementVariablesWorldToLocal();
				Velocity = NewFallVelocity(Velocity, FVector(0.f, 0.f, GetGravityZ()), DeltaSeconds);
				TransformMovementVariablesLocalToWorld();
				SetMovementMode(MOVE_Falling);
			}
			else
			{
				// Walkable floor
				if (MovementMode == MOVE_Walking)
				{
					AdjustFloorHeight();
					SetBase(CurrentFloor.HitResult.Component.Get());
				}
				else if (MovementMode == MOVE_Falling)
				{
					if (CurrentFloor.FloorDist <= MIN_FLOOR_DIST)
					{
						// Landed
						SetMovementMode(MOVE_Walking);
					}
					else
					{
						// Continue falling.
						TransformMovementVariablesWorldToLocal();
						Velocity = NewFallVelocity(Velocity, FVector(0.f, 0.f, GetGravityZ()), DeltaSeconds);
						TransformMovementVariablesLocalToWorld();
						CurrentFloor.Clear();
					}
				}
			}
		}

		OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	} // End scoped movement update

	// Call custom post-movement events. These happen after the scoped movement completes in case the events want to use the current state of overlaps etc.
	CallMovementUpdateDelegate(DeltaSeconds, OldLocation, OldVelocity);

	SaveBaseLocation();
	UpdateComponentVelocity();
	bJustTeleported = false;

	LastUpdateLocation = UpdatedComponent ? UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
}

void UAdvKitGravityCharacterMovementComponent::StartFalling(int32 Iterations, float remainingTime, float timeTick, const FVector& Delta, const FVector& subLoc)
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		Super::StartFalling(Iterations, remainingTime, timeTick, Delta, subLoc);
		return;
	}

	// start falling 
	const float DesiredDist = Delta.Size();
	const float ActualDist = GetOrientationComp()->TransformDirectionW2L(CharacterOwner->GetActorLocation() - subLoc).Size2D(); //Changed
	remainingTime = (DesiredDist == 0.f)
		? 0.f
		: remainingTime + timeTick * (1.f - FMath::Min(1.f, ActualDist / DesiredDist));

	//Changed
	Velocity = GetOrientationComp()->TransformDirectionW2L(Velocity);
	Velocity.Z = 0.f;
	Velocity = GetOrientationComp()->TransformDirectionL2W(Velocity);

	if (IsMovingOnGround())
	{
		// This is to catch cases where the first frame of PIE is executed, and the
		// level is not yet visible. In those cases, the player will fall out of the
		// world... So, don't set MOVE_Falling straight away.
		if (!GIsEditor || (GetWorld()->HasBegunPlay() && (GetWorld()->GetTimeSeconds() >= 1.f)))
		{
			SetAdvMovementMode(EAdvKitMovementMode::Falling); //default behavior if script didn't change physics
		}
		else
		{
			// Make sure that the floor check code continues processing during this delay.
			bForceNextFloorCheck = true;
		}
	}
	StartNewPhysics(remainingTime, Iterations);
}

void UAdvKitGravityCharacterMovementComponent::PerformAirControlForPathFollowing(FVector Direction, float ZDiff)
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		Super::PerformAirControlForPathFollowing(Direction, ZDiff);
		return;
	}

	Velocity = GetOrientationComp()->TransformDirectionW2L(Velocity); //Changed
	Direction = GetOrientationComp()->TransformDirectionW2L(Direction); //Changed

	// use air control if low grav or above destination and falling towards it
	if (CharacterOwner && Velocity.Z < 0.f && (ZDiff < 0.f || GetGravityZ() > 0.9f * GetWorld()->GetDefaultGravityZ()))
	{
		if (ZDiff > 0.f)
		{
			if (ZDiff > 2.f * GetMaxJumpHeight())
			{
				if (PathFollowingComp.IsValid())
				{
					PathFollowingComp->AbortMove(TEXT("missed jump"));
				}
			}
		}
		else
		{
			if ((Velocity.X == 0.f) && (Velocity.Y == 0.f))
			{
				Acceleration = FVector::ZeroVector;
			}
			else
			{
				float Dist2D = Direction.Size2D();
				//Direction.Z = 0.f;
				Acceleration = Direction.GetSafeNormal() * GetMaxAcceleration();

				if ((Dist2D < 0.5f * FMath::Abs(Direction.Z)) && ((Velocity | Direction) > 0.5f*FMath::Square(Dist2D)))
				{
					Acceleration *= -1.f;
				}

				if (Dist2D < 1.5f*CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius())
				{
					Velocity.X = 0.f;
					Velocity.Y = 0.f;
					Acceleration = FVector::ZeroVector;
				}
				else if ((Velocity | Direction) < 0.f)
				{
					float M = FMath::Max(0.f, 0.2f - GetWorld()->DeltaTimeSeconds);
					Velocity.X *= M;
					Velocity.Y *= M;
				}
			}
		}
	}

	Velocity = GetOrientationComp()->TransformDirectionL2W(Velocity); //Changed
}

void UAdvKitGravityCharacterMovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		Super::RequestDirectMove(MoveVelocity, bForceMaxSpeed);
		return;
	}

	if (MoveVelocity.SizeSquared() < KINDA_SMALL_NUMBER)
	{
		return;
	}

	if (IsFalling())
	{
		const FVector FallVelocity = MoveVelocity.GetClampedToMaxSize(GetMaxSpeed());
		PerformAirControlForPathFollowing(FallVelocity, GetOrientationComp()->TransformDirectionW2L(FallVelocity).Z); //Changed
		return;
	}

	RequestedVelocity = MoveVelocity;
	bHasRequestedVelocity = true;
	bRequestedMoveWithMaxSpeed = bForceMaxSpeed;

	if (IsMovingOnGround())
	{
		RequestedVelocity = GetOrientationComp()->TransformDirectionW2L(RequestedVelocity); //Changed
		RequestedVelocity.Z = 0.0f;
		RequestedVelocity = GetOrientationComp()->TransformDirectionL2W(RequestedVelocity); //Changed
	}
}

void UAdvKitGravityCharacterMovementComponent::AdjustFloorHeight()
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		Super::AdjustFloorHeight();
		return;
	}

	// If we have a floor check that hasn't hit anything, don't adjust height.
	if (!CurrentFloor.bBlockingHit)
	{
		return;
	}

	const float OldFloorDist = CurrentFloor.FloorDist;
	if (CurrentFloor.bLineTrace && OldFloorDist < MIN_FLOOR_DIST)
	{
		// This would cause us to scale unwalkable walls
		return;
	}

	// Move up or down to maintain floor height.
	if (OldFloorDist < MIN_FLOOR_DIST || OldFloorDist > MAX_FLOOR_DIST)
	{
		FHitResult AdjustHit(1.f);
		const float LocalInitialZ = GetOrientationComp()->TransformPositionW2L(UpdatedComponent->GetComponentLocation()).Z; //Changed
		const float AvgFloorDist = (MIN_FLOOR_DIST + MAX_FLOOR_DIST) * 0.5f;
		const float MoveDist = AvgFloorDist - OldFloorDist;
		SafeMoveUpdatedComponent(GetOrientationComp()->TransformDirectionL2W(FVector(0.f, 0.f, MoveDist)), CharacterOwner->GetActorRotation(), true, AdjustHit); //Changed
		UE_LOG(LogAdvKitGravityCharacterMovement, VeryVerbose, TEXT("Adjust floor height %.3f (Hit = %d)"), MoveDist, AdjustHit.bBlockingHit); //Changed

		if (!AdjustHit.IsValidBlockingHit())
		{
			CurrentFloor.FloorDist += MoveDist;
		}
		else if (MoveDist > 0.f)
		{
			// If moving up, use the actual impact location, not the pulled back time/location.
			const float FloorDistTime = FMath::Abs((LocalInitialZ - GetOrientationComp()->TransformPositionW2L(AdjustHit.Location).Z) / MoveDist); //Changed
			CurrentFloor.FloorDist += MoveDist * FloorDistTime;
		}
		else
		{
			check(MoveDist < 0.f);
			const float LocalCurrentZ = GetOrientationComp()->TransformPositionW2L(UpdatedComponent->GetComponentLocation()).Z; //Changed
			CurrentFloor.FloorDist = LocalCurrentZ - GetOrientationComp()->TransformPositionW2L(AdjustHit.Location).Z;
			if (IsWalkable(AdjustHit))
			{
				SetFromSweepHACK(CurrentFloor, AdjustHit, CurrentFloor.FloorDist, true); //Changed
				//CurrentFloor.SetFromSweep(AdjustHit, CurrentFloor.FloorDist, true);//Changed
			}
		}

		// Don't recalculate velocity based on this height adjustment, if considering vertical adjustments.
		// Also avoid it if we moved out of penetration
		bJustTeleported |= !bMaintainHorizontalGroundVelocity || (OldFloorDist < 0.f);
	}
}

bool UAdvKitGravityCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	//Without an orientation component use default implementation
	if (AdvKitGravityCharacterOwner)
	{
		switch (GetAdvMovementMode())
		{
		case EAdvKitMovementMode::Swimming:
		{
			TransformMovementVariablesWorldToLocal(); //Changed
			Velocity.Z = JumpZVelocity;
			TransformMovementVariablesLocalToWorld(); //Changed

			// just left the water - check if should jump out
			SetMovementMode(MOVE_Falling);
			FVector JumpDir(0.f);
			FVector WallNormal(0.f);
			if (Acceleration.Z > 0.f && ShouldJumpOutOfWater(JumpDir)
				&& ((JumpDir | Acceleration) > 0.f) && CheckWaterJump(JumpDir, WallNormal))
			{
				JumpOutOfWater(WallNormal);
				Velocity.Z = OutofWaterZ; //set here so physics uses this for remainder of tick
			}
			break;
		}
		case EAdvKitMovementMode::Walking:
			// Don't jump if we can't move up/down.
			if (FMath::Abs(PlaneConstraintNormal.Z) != 1.f)
			{
				TransformMovementVariablesWorldToLocal(); //Changed
				Velocity.Z = JumpZVelocity;
				TransformMovementVariablesLocalToWorld(); //Changed

				//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Velocity*0.5f, FColor::Green, true);
				TArray<FHitResult> Hits;
				FComponentQueryParams Params;
				FVector JumpTarget = GetActorLocation() + Velocity*0.5f;
				if (GetWorld()->ComponentSweepMulti(Hits, AdvKitGravityCharacterOwner->GetCapsuleComponent(), GetActorLocation(), JumpTarget, UpdatedComponent->GetComponentRotation(), Params))
				{
					for (FHitResult Hit : Hits)
					{
						AAdvKitZone* Zone = Cast<AAdvKitZone>(Hit.Actor.Get());
						if (!Zone)
						{
							continue;
						}

						if (!Zone->HasPhysics(EAdvKitMovementMode::ClimbingLedge))
						{
							continue;
						}

						if (AdvKitGravityCharacterOwner->StartTransitionActionTo(Zone->GetPhysics(), UAdvKitCA_ZoneTransition::MakeArguments(Zone)))
						{
							return true;
						}
					}
				}

				SetMovementMode(MOVE_Falling);
				return true;
			}
		}
	}

	return Super::DoJump(bReplayingMoves);
}

void UAdvKitGravityCharacterMovementComponent::PhysFalling(float DeltaSeconds, int32 Iterations)
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		Super::PhysFalling(DeltaSeconds, Iterations);
		return;
	}
	TransformMovementVariablesWorldToLocal(); //Changed

	// Bound final 2d portion of velocity
	const float Speed2d = Velocity.Size2D();
	const float BoundSpeed = FMath::Max(Speed2d, GetMaxSpeed() * AnalogInputModifier);

	//bound acceleration, falling object has minimal ability to impact acceleration
	FVector FallAcceleration = Acceleration;
	FallAcceleration.Z = 0.f;

	FHitResult Hit(1.f);
	if (!(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()))
	{
		// test for slope to avoid using air control to climb walls
		float TickAirControl = AirControl;
		if (TickAirControl > 0.0f && FallAcceleration.SizeSquared() > 0.f)
		{
			const float TestWalkTime = FMath::Max(DeltaSeconds, 0.05f);
			const FVector TestWalk = ((TickAirControl * GetMaxAcceleration() * GetOrientationComp()->TransformDirectionL2W(FallAcceleration.GetSafeNormal()) + GetOrientationComp()->GetUpVector()*GetGravityZ()) * TestWalkTime + GetOrientationComp()->TransformDirectionL2W(Velocity)) * TestWalkTime; //Changed
			if (!TestWalk.IsZero())
			{
				static const FName FallingTraceParamsTag = FName(TEXT("PhysFalling"));
				FHitResult Result(1.f);
				FCollisionQueryParams CapsuleQuery(FallingTraceParamsTag, false, CharacterOwner);
				FCollisionResponseParams ResponseParam;
				InitCollisionParams(CapsuleQuery, ResponseParam);
				const FVector PawnLocation = CharacterOwner->GetActorLocation();
				const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

				bool bHit = false;
				const FCollisionShape CapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_None);
				if (bUseFlatBaseForFloorChecks)
				{
					const FCollisionShape BoxShape = FCollisionShape::MakeBox(FVector(CapsuleShape.GetCapsuleRadius(), CapsuleShape.GetCapsuleRadius(), CapsuleShape.GetCapsuleHalfHeight()));
					GetWorld()->SweepSingleByChannel(Result, PawnLocation, PawnLocation + TestWalk, UpdatedComponent->GetComponentQuat(), CollisionChannel, BoxShape, CapsuleQuery, ResponseParam); //Changed
				}
				else
				{
					GetWorld()->SweepSingleByChannel(Result, PawnLocation, PawnLocation + TestWalk, UpdatedComponent->GetComponentQuat(), CollisionChannel, CapsuleShape, CapsuleQuery, ResponseParam); //Changed
				}

				if (bHit)
				{
					// Only matters if we can't walk there
					if (!IsValidLandingSpot(Result.Location, Result))
					{
						TickAirControl = 0.f;
					}
				}
			}
		}

		float MaxAccel = GetMaxAcceleration() * TickAirControl;

		// Boost maxAccel to increase player's control when falling
		if ((Speed2d < 10.f) && (TickAirControl > 0.f) && (TickAirControl <= 0.05f)) //allow initial burst
		{
			MaxAccel = MaxAccel + (10.f - Speed2d) / DeltaSeconds;
		}

		FallAcceleration = FallAcceleration.GetClampedToMaxSize(MaxAccel);
	}

	float remainingTime = DeltaSeconds;
	float timeTick = 0.1f;


	while ((remainingTime > 0.f) && (Iterations < 8))
	{
		Iterations++;
		timeTick = (remainingTime > 0.05f)
			? FMath::Min(0.05f, remainingTime * 0.5f)
			: remainingTime;

		remainingTime -= timeTick;
		const FVector OldLocation = CharacterOwner->GetActorLocation();
		const FRotator PawnRotation = CharacterOwner->GetActorRotation();
		bJustTeleported = false;

		FVector OldVelocity = Velocity;

		// Apply input
		if (!(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()))
		{
			// Acceleration = FallAcceleration for CalcVelocity, but we restore it after using it.
			TGuardValue<FVector> RestoreAcceleration(Acceleration, FallAcceleration);
			const float SavedVelZ = Velocity.Z;
			Velocity.Z = 0.f;
			TransformMovementVariablesLocalToWorld(); //Changed
			//CalcVelocity(timeTick, FallingLateralFriction, false, BrakingDecelerationFalling); //TODO Convert
			TransformMovementVariablesWorldToLocal(); //Changed
			Velocity.Z = SavedVelZ;
		}

		// Apply gravity
		Velocity = NewFallVelocity(Velocity, FVector(0.f, 0.f, GetGravityZ()), timeTick);

		if (bNotifyApex && CharacterOwner->Controller && (Velocity.Z <= 0.f))
		{
			// Just passed jump apex since now going down
			bNotifyApex = false;
			NotifyJumpApex();
		}

		if (!(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()))
		{
			// make sure not exceeding acceptable speed
			Velocity = Velocity.GetClampedToMaxSize2D(BoundSpeed);
		}

		FVector Adjusted = 0.5f*(OldVelocity + Velocity) * timeTick;
		SafeMoveUpdatedComponent(GetOrientationComp()->TransformDirectionL2W(Adjusted), PawnRotation, true, Hit);


		if (!HasValidData())
		{
			TransformMovementVariablesLocalToWorld(); //Changed
			return;
		}

		if (IsSwimming()) //just entered water
		{
			remainingTime = remainingTime + timeTick * (1.f - Hit.Time);
			TransformMovementVariablesLocalToWorld(); //Changed
			StartSwimmingHACK(OldLocation, GetOrientationComp()->TransformDirectionL2W(OldVelocity), timeTick, remainingTime, Iterations); //Changed
			return;
		}
		else if (Hit.Time < 1.f)
		{
			if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
			{
				remainingTime += timeTick * (1.f - Hit.Time);
				if (!bJustTeleported && (Hit.Time > 0.1f) && (Hit.Time * timeTick > 0.003f))
				{
					Velocity = GetOrientationComp()->TransformDirectionW2L(CharacterOwner->GetActorLocation() - OldLocation) / (timeTick * Hit.Time); //Changed
				}
				TransformMovementVariablesLocalToWorld();
				ProcessLanded(Hit, remainingTime, Iterations);
				return;
			}
			else
			{
				HandleImpact(Hit, DeltaSeconds, GetOrientationComp()->TransformDirectionL2W(Adjusted)); //Changed

				if (Acceleration.SizeSquared2D() > 0.f)
				{
					// If we've changed physics mode, abort.
					if (!HasValidData() || !IsFalling())
					{
						TransformMovementVariablesLocalToWorld();
						return;
					}
				}

				const FVector OldHitNormal = Hit.Normal;
				const FVector OldHitImpactNormal = Hit.ImpactNormal;
				FVector Delta = ComputeSlideVector(GetOrientationComp()->TransformDirectionL2W(Adjusted), 1.f - Hit.Time, OldHitNormal, Hit); //Changed

				if ((GetOrientationComp()->TransformDirectionW2L(Delta) | Adjusted) > 0.f) //Changed
				{
					SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit); //Changed

					if (Hit.Time < 1.f) //hit second wall
					{
						if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
						{
							TransformMovementVariablesLocalToWorld();  //Changed
							remainingTime = 0.f;
							ProcessLanded(Hit, remainingTime, Iterations);
							return;
						}

						HandleImpact(Hit, timeTick, Delta);

						// If we've changed physics mode, abort.
						if (!HasValidData() || !IsFalling())
						{
							TransformMovementVariablesLocalToWorld(); //Changed
							return;
						}

						TwoWallAdjust(Delta, Hit, OldHitNormal);

						// bDitch=true means that pawn is straddling two slopes, neither of which he can stand on
						bool bDitch = ((GetOrientationComp()->TransformDirectionW2L(OldHitImpactNormal).Z > 0.f) && (GetOrientationComp()->TransformDirectionW2L(Hit.ImpactNormal).Z > 0.f) && (FMath::Abs(GetOrientationComp()->TransformDirectionW2L(Delta).Z) <= KINDA_SMALL_NUMBER) && ((Hit.ImpactNormal | OldHitImpactNormal) < 0.f)); //Changed
						SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);

						if (Hit.Time == 0)
						{
							// if we are stuck then try to side step
							FVector SideDelta = GetOrientationComp()->TransformDirectionW2L(OldHitNormal + Hit.ImpactNormal).GetSafeNormal2D(); //Changed
							if (SideDelta.IsNearlyZero())
							{
								FVector LocalOldHitNormal = GetOrientationComp()->TransformDirectionW2L(OldHitNormal); //Changed
								SideDelta = FVector(LocalOldHitNormal.Y, -LocalOldHitNormal.X, 0).GetSafeNormal(); //Changed
							}
							SafeMoveUpdatedComponent(GetOrientationComp()->TransformDirectionL2W(SideDelta), PawnRotation, true, Hit); //Changed
						}

						if (bDitch || IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit) || Hit.Time == 0)
						{
							TransformMovementVariablesLocalToWorld(); //Changed
							remainingTime = 0.f;
							ProcessLanded(Hit, remainingTime, Iterations);
							return;
						}
						else if (GetPerchRadiusThreshold() > 0.f && Hit.Time == 1.f && GetOrientationComp()->TransformDirectionW2L(OldHitImpactNormal).Z >= GetWalkableFloorZ())
						{
							// We might be in a virtual 'ditch' within our perch radius. This is rare.
							const FVector PawnLocation = CharacterOwner->GetActorLocation();
							const float ZMovedDist = FMath::Abs(GetOrientationComp()->TransformPositionW2L(PawnLocation - OldLocation).Z); //Changed
							const float MovedDist2DSq = GetOrientationComp()->TransformPositionW2L(PawnLocation - OldLocation).SizeSquared2D(); //Changed
							if (ZMovedDist <= 0.2f * timeTick && MovedDist2DSq <= 4.f * timeTick)
							{
								Velocity.X += 0.25f * GetMaxSpeed() * (FMath::FRand() - 0.5f);
								Velocity.Y += 0.25f * GetMaxSpeed() * (FMath::FRand() - 0.5f);
								Velocity.Z = FMath::Max<float>(JumpZVelocity * 0.25f, 1.f);
								Delta = GetOrientationComp()->TransformDirectionL2W(Velocity) * timeTick; //Changed
								SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);
							}
						}
					}
				}

				// Calculate average velocity based on actual movement after considering collisions
				if (!bJustTeleported)
				{
					// Use average velocity for XY movement (no acceleration except for air control in those axes), but want actual velocity in Z axis
					const float OldVelZ = OldVelocity.Z;
					OldVelocity = GetOrientationComp()->TransformDirectionW2L(CharacterOwner->GetActorLocation() - OldLocation) / timeTick; //Changed
					OldVelocity.Z = OldVelZ;
				}
			}
		}

		if (!(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()) && !bJustTeleported && MovementMode != MOVE_None)
		{
			// refine the velocity by figuring out the average actual velocity over the tick, and then the final velocity.
			// This particularly corrects for situations where level geometry affected the fall.
			Velocity = GetOrientationComp()->TransformDirectionW2L(CharacterOwner->GetActorLocation() - OldLocation) / timeTick; //actual average velocity //Changed
			if ((Velocity.Z < OldVelocity.Z) || (OldVelocity.Z >= 0.f))
			{
				Velocity = 2.f*Velocity - OldVelocity; //end velocity has 2* accel of avg
			}

			if (Velocity.SizeSquared2D() <= KINDA_SMALL_NUMBER * 10.f)
			{
				Velocity.X = 0.f;
				Velocity.Y = 0.f;
			}

			Velocity = Velocity.GetClampedToMaxSize(GetPhysicsVolume()->TerminalVelocity);
		}
	}

	TransformMovementVariablesLocalToWorld(); //Changed
}

void UAdvKitGravityCharacterMovementComponent::StartSwimmingHACK(FVector OldLocation, FVector OldVelocity, float timeTick, float remainingTime, int32 Iterations)
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		Super::StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
		return;
	}

	if (!(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()) && !bJustTeleported)
	{
		if (timeTick > 0.f)
		{
			Velocity = (CharacterOwner->GetActorLocation() - OldLocation) / timeTick; //actual average velocity
		}
		Velocity = 2.f*Velocity - OldVelocity; //end velocity has 2* accel of avg
		if (Velocity.SizeSquared() > FMath::Square(GetPhysicsVolume()->TerminalVelocity))
		{
			Velocity = Velocity.GetSafeNormal();
			Velocity *= GetPhysicsVolume()->TerminalVelocity;
		}
	}
	FVector End = FindWaterLine(CharacterOwner->GetActorLocation(), OldLocation);
	float waterTime = 0.f;
	if (End != CharacterOwner->GetActorLocation())
	{
		waterTime = timeTick * (End - CharacterOwner->GetActorLocation()).Size() / (CharacterOwner->GetActorLocation() - OldLocation).Size();
		remainingTime += waterTime;
		MoveUpdatedComponent(End - CharacterOwner->GetActorLocation(), CharacterOwner->GetActorRotation(), true);
	}

	FVector LocalVelocity = GetOrientationComp()->TransformDirectionW2L(Velocity);
	if (!(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()) && CharacterOwner && (LocalVelocity.Z > 2.f*SWIMBOBSPEED) && (LocalVelocity.Z < 0.f)) //allow for falling out of water //Changed
	{
		LocalVelocity.Z = SWIMBOBSPEED - LocalVelocity.Size2D() * 0.7f; //smooth bobbing
		Velocity = GetOrientationComp()->TransformDirectionL2W(LocalVelocity);
	}
	if ((remainingTime > 0.01f) && (Iterations < 8) && CharacterOwner)
	{
		PhysSwimming(remainingTime, Iterations);
	}
}

float UAdvKitGravityCharacterMovementComponent::ImmersionDepthGravity()
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		return ImmersionDepth();
	}


	float depth = 0.f;

	if (CharacterOwner && GetPhysicsVolume()->bWaterVolume)
	{
		const float CollisionHeight = CharacterOwner->GetSimpleCollisionRadius();
		const float CollisionRadius = CharacterOwner->GetSimpleCollisionHalfHeight();

		if ((CollisionHeight == 0.f) || (Buoyancy == 0.f))
		{
			depth = 1.f;
		}
		else
		{
			UBrushComponent* VolumeBrushComp = GetPhysicsVolume()->BrushComponent;
			FHitResult Hit(1.f);
			if (VolumeBrushComp)
			{
				const FVector TraceStart = CharacterOwner->GetActorLocation() + GetOrientationComp()->GetUpVector()*CollisionHeight; //Changed
				const FVector TraceEnd = CharacterOwner->GetActorLocation() - GetOrientationComp()->GetUpVector()*CollisionHeight; //Changed

				const static FName MovementComp_Character_ImmersionDepthName(TEXT("MovementComp_Character_ImmersionDepth"));
				FCollisionQueryParams NewTraceParams(MovementComp_Character_ImmersionDepthName, true);

				VolumeBrushComp->LineTraceComponent(Hit, TraceStart, TraceEnd, NewTraceParams);
			}

			depth = (Hit.Time == 1.f) ? 1.f : (1.f - Hit.Time);
		}
	}
	return depth;
}

void UAdvKitGravityCharacterMovementComponent::PhysSwimming(float DeltaSeconds, int32 Iterations)
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		return PhysSwimming(DeltaSeconds, Iterations);
	}

	if (DeltaSeconds < MIN_TICK_TIME)
	{
		return;
	}

	const FVector CharacterLocation = CharacterOwner->GetActorLocation();// GetOrientationComp()->TransformPositionW2L(CharacterOwner->GetActorLocation());

	TransformMovementVariablesWorldToLocal(); //Changed

	float NetFluidFriction = 0.f;
	float Depth = ImmersionDepthGravity();
	float NetBuoyancy = Buoyancy * Depth;

	if (!(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()) && (Velocity.Z > 0.5f*GetMaxSpeed()) && (NetBuoyancy != 0.f))
	{
		//damp positive Z out of water
		Velocity.Z = Velocity.Z * Depth;
	}
	Iterations++;
	FVector OldLocation = GetOrientationComp()->TransformPositionW2L(CharacterOwner->GetActorLocation()); //Changed
	bJustTeleported = false;
	if (!(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()))
	{
		const float Friction = 0.5f * GetPhysicsVolume()->FluidFriction * Depth;
		TransformMovementVariablesLocalToWorld(); //Changed
		CalcVelocity(DeltaSeconds, Friction, true, BrakingDecelerationSwimming);
		TransformMovementVariablesWorldToLocal(); //Changed
		Velocity.Z += GetGravityZ() * DeltaSeconds * (1.f - NetBuoyancy);
	}



	FVector Adjusted = GetOrientationComp()->TransformDirectionL2W(Velocity * DeltaSeconds); //Changed
	FHitResult Hit(1.f);
	float remainingTime = DeltaSeconds * Swim(Adjusted, Hit);

	//may have left water - if so, script might have set new physics mode
	if (!IsSwimming())
	{
		TransformMovementVariablesLocalToWorld(); //Changed
		StartNewPhysics(remainingTime, Iterations);
		return;
	}

	if (Hit.Time < 1.f && CharacterOwner)
	{
		const FVector GravDir = FVector(0.f, 0.f, -1.f);
		const FVector VelDir = Velocity.GetSafeNormal();
		const float UpDown = GravDir | VelDir;

		bool bSteppedUp = false;
		if ((FMath::Abs(Hit.ImpactNormal.Z) < 0.2f) && (UpDown < 0.5f) && (UpDown > -0.2f) && CanStepUp(Hit))
		{
			float stepZ = GetOrientationComp()->TransformPositionW2L(CharacterOwner->GetActorLocation()).Z; //Changed
			const FVector RealVelocity = Velocity;
			Velocity.Z = 1.f;	// HACK: since will be moving up, in case pawn leaves the water
			TransformMovementVariablesLocalToWorld(); //Changed
			bSteppedUp = StepUp(GetOrientationComp()->TransformDirectionL2W(GravDir), Adjusted * (1.f - Hit.Time), Hit); //Changed
			if (bSteppedUp)
			{
				//may have left water - if so, script might have set new physics mode
				if (!IsSwimming())
				{
					StartNewPhysics(remainingTime, Iterations);
					return;
				}

				//OldLocation.Z = GetOrientationComp()->TransformPositionW2L(CharacterOwner->GetActorLocation()).Z + (OldLocation.Z - stepZ); //Changed
			}

			TransformMovementVariablesWorldToLocal(); //Changed

			Velocity = RealVelocity;
		}

		if (!bSteppedUp)
		{
			//adjust and try again
			HandleImpact(Hit, DeltaSeconds, Adjusted);
			SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
		}
	}

	if (!(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()) && !bJustTeleported && ((DeltaSeconds - remainingTime) > KINDA_SMALL_NUMBER) && CharacterOwner)
	{
		bool bWaterJump = !GetPhysicsVolume()->bWaterVolume;
		float velZ = Velocity.Z;
		Velocity = (GetOrientationComp()->TransformPositionW2L(CharacterOwner->GetActorLocation()) - OldLocation) / (DeltaSeconds - remainingTime); //Changed
		if (bWaterJump)
		{
			Velocity.Z = velZ;
		}
	}

	TransformMovementVariablesLocalToWorld(); //Changed

	if (!GetPhysicsVolume()->bWaterVolume && IsSwimming())
	{
		SetMovementMode(MOVE_Falling); //in case script didn't change it (w/ zone change)
	}

	//may have left water - if so, script might have set new physics mode
	if (!IsSwimming())
	{
		StartNewPhysics(remainingTime, Iterations);
	}
}

FVector UAdvKitGravityCharacterMovementComponent::ConstrainInputAcceleration(const FVector& InputAcceleration) const
{
	//Without an orientation component use default implementation
	auto Orientation = GetOrientationComp();
	if (!Orientation)
	{
		return Super::ConstrainInputAcceleration(InputAcceleration);
	}

	FVector NewAccel = InputAcceleration;

	// walking or falling pawns ignore up/down sliding
	if (IsMovingOnGround() || IsFalling())
	{
		NewAccel = GetOrientationComp()->TransformDirectionW2L(NewAccel); //Changed
		NewAccel.Z = 0.f;
		NewAccel = GetOrientationComp()->TransformDirectionL2W(NewAccel); //Changed
	}

	return NewAccel;
}

FVector UAdvKitGravityCharacterMovementComponent::ComputeGroundMovementDelta(const FVector& Delta, const FHitResult& RampHit, const bool bHitFromLineTrace) const
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		return Super::ComputeGroundMovementDelta(Delta, RampHit, bHitFromLineTrace);
	}

	FVector LocalDelta = GetOrientationComp()->TransformDirectionW2L(Delta);
	const FVector FloorNormal = GetOrientationComp()->TransformDirectionW2L(RampHit.ImpactNormal); //Changed
	const FVector ContactNormal = GetOrientationComp()->TransformDirectionW2L(RampHit.Normal); //Changed

	if (FloorNormal.Z < (1.f - KINDA_SMALL_NUMBER) && FloorNormal.Z > KINDA_SMALL_NUMBER && ContactNormal.Z > KINDA_SMALL_NUMBER && !bHitFromLineTrace && IsWalkable(RampHit))
	{
		const float FloorDotDelta = (FloorNormal | LocalDelta);
		FVector RampMovement(LocalDelta.X, LocalDelta.Y, -FloorDotDelta / FloorNormal.Z);

		if (bMaintainHorizontalGroundVelocity)
		{
			return GetOrientationComp()->TransformDirectionL2W(RampMovement);
		}
		else
		{
			return GetOrientationComp()->TransformDirectionL2W(RampMovement.GetSafeNormal() * LocalDelta.Size());
		}
	}

	return Delta;
}

void UAdvKitGravityCharacterMovementComponent::MoveSmooth(const FVector& InVelocity, const float DeltaSeconds, FStepDownResult* OutStepDownResult)
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		Super::MoveSmooth(InVelocity, DeltaSeconds, OutStepDownResult);
		return;
	}

	FVector Delta = InVelocity * DeltaSeconds;
	if (!HasValidData() || Delta.IsZero())
	{
		return;
	}

	FScopedMovementUpdate ScopedMovementUpdate(UpdatedComponent, bEnableScopedMovementUpdates ? EScopedUpdate::DeferredUpdates : EScopedUpdate::ImmediateUpdates);

	if (IsMovingOnGround())
	{
		MoveAlongFloor(InVelocity, DeltaSeconds, OutStepDownResult);
	}
	else
	{
		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Delta, CharacterOwner->GetActorRotation(), true, Hit);

		if (Hit.IsValidBlockingHit())
		{
			bool bSteppedUp = false;

			if (IsFlying())
			{
				if (CanStepUp(Hit))
				{
					OutStepDownResult = NULL; // No need for a floor when not walking.
					if (FMath::Abs(GetOrientationComp()->TransformDirectionW2L(Hit.ImpactNormal).Z) < 0.2f) //Changed
					{
						const FVector GravDir = -GetOrientationComp()->GetUpVector(); //Changed
						const FVector DesiredDir = Delta.GetSafeNormal();
						const float UpDown = GravDir | DesiredDir;
						if ((UpDown < 0.5f) && (UpDown > -0.2f))
						{
							bSteppedUp = StepUp(GravDir, Delta * (1.f - Hit.Time), Hit, OutStepDownResult);
						}
					}
				}
			}

			// If StepUp failed, try sliding.
			if (!bSteppedUp)
			{
				SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, false);
			}
		}
	}
}

bool UAdvKitGravityCharacterMovementComponent::IsWalkable(const FHitResult& Hit) const
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		return Super::IsWalkable(Hit);
	}

	if (!Hit.bBlockingHit)
	{
		return false;
	}

	FVector LocalHitNormal = GetOrientationComp()->TransformDirectionW2L(Hit.ImpactNormal); //Changed

	// Never walk up vertical surfaces.
	if (LocalHitNormal.Z < KINDA_SMALL_NUMBER) //Changed
	{
		return false;
	}

	float TestWalkableZ = GetWalkableFloorZ();

	//TODO Convert
	//// See if this component overrides the walkable floor z.
	const UPrimitiveComponent* HitComponent = Hit.Component.Get();
	if (HitComponent)
	{
		if (!HitComponent->CanCharacterStepUp(CharacterOwner)) //Changed TODO is it correct to use this here?
		{
			return false;
		}
	//	const FWalkableSlopeoverride& Slopeoverride = HitComponent->GetWalkableSlopeoverride(); //TODO
	//	TestWalkableZ = Slopeoverride.ModifyWalkableFloorZ(TestWalkableZ);
	}

	// Can't walk on this surface if it is too steep.
	if (LocalHitNormal.Z < TestWalkableZ) //Changed
	{
		return false;
	}

	return true;
}

void UAdvKitGravityCharacterMovementComponent::MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds, FStepDownResult* OutStepDownResult)
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		return Super::MoveAlongFloor(InVelocity, DeltaSeconds, OutStepDownResult);
	}

	const FVector Delta = GetOrientationComp()->TransformDirectionL2W(FVector(1, 1, 0.f) * GetOrientationComp()->TransformDirectionW2L(InVelocity) * DeltaSeconds); //Changed

	if (!CurrentFloor.IsWalkableFloor())
	{
		return;
	}

	FHitResult Hit(1.f);
	FVector RampVector = ComputeGroundMovementDelta(Delta, CurrentFloor.HitResult, CurrentFloor.bLineTrace);
	SafeMoveUpdatedComponent(RampVector, CharacterOwner->GetActorRotation(), true, Hit);
	if (Hit.bStartPenetrating)
	{
		//UE_LOG(LogCharacterMovement, Log, TEXT("%s is stuck and failed to move!"), *CharacterOwner->GetName()); //Changed

		// Don't update velocity based on our (failed) change in position this update since we're stuck.
		bJustTeleported = true;
	}

	if (Hit.IsValidBlockingHit())
	{
		// See if we impacted something (most likely another ramp, but possibly a barrier). Try to slide along it as well.
		float TimeApplied = Hit.Time;
		if ((Hit.Time > 0.f) && (GetOrientationComp()->TransformDirectionW2L(Hit.Normal).Z > KINDA_SMALL_NUMBER) && IsWalkable(Hit)) //Changed
		{
			const float PreSlideTimeRemaining = 1.f - Hit.Time;
			RampVector = ComputeGroundMovementDelta(Delta * PreSlideTimeRemaining, Hit, false);
			SafeMoveUpdatedComponent(RampVector, CharacterOwner->GetActorRotation(), true, Hit);

			const float SecondHitPercent = Hit.Time * (1.f - TimeApplied);
			TimeApplied = FMath::Clamp(TimeApplied + SecondHitPercent, 0.f, 1.f);
		}

		if (Hit.IsValidBlockingHit())
		{
			if (CanStepUp(Hit) || (CharacterOwner->GetMovementBase() != NULL && CharacterOwner->GetMovementBase()->GetOwner() == Hit.GetActor()))
			{
				// hit a barrier, try to step up
				const FVector GravDir = -GetOrientationComp()->GetUpVector(); //Changed
				if (!StepUp(GravDir, Delta * (1.f - TimeApplied), Hit, OutStepDownResult))
				{
					//UE_LOG(LogCharacterMovement, Verbose, TEXT("- StepUp (ImpactNormal %s, Normal %s"), *Hit.ImpactNormal.ToString(), *Hit.Normal.ToString()); //Changed
					HandleImpact(Hit, DeltaSeconds, Delta);
					SlideAlongSurface(Delta, 1.f - TimeApplied, Hit.Normal, Hit, true);
				}
				else
				{
					// Don't recalculate velocity based on this height adjustment, if considering vertical adjustments.
					//UE_LOG(LogCharacterMovement, Verbose, TEXT("+ StepUp (ImpactNormal %s, Normal %s"), *Hit.ImpactNormal.ToString(), *Hit.Normal.ToString()); //Changed
					bJustTeleported |= !bMaintainHorizontalGroundVelocity;
				}
			}
			else if (Hit.Component.IsValid() && !Hit.Component.Get()->CanCharacterStepUp(CharacterOwner))
			{
				HandleImpact(Hit, DeltaSeconds, Delta);
				SlideAlongSurface(Delta, 1.f - TimeApplied, Hit.Normal, Hit, true);
			}
		}
	}
}

float UAdvKitGravityCharacterMovementComponent::SlideAlongSurface(const FVector& Delta, float Time, const FVector& InNormal, FHitResult &Hit, bool bHandleImpact)
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		return Super::SlideAlongSurface(Delta, Time, InNormal, Hit, bHandleImpact);
	}

	if (!Hit.bBlockingHit)
	{
		return 0.f;
	}

	FVector Normal = GetOrientationComp()->TransformDirectionW2L(InNormal); //Changed
	if (IsMovingOnGround())
	{
		// We don't want to be pushed up an unwalkable surface.
		if (Normal.Z > 0.f)
		{
			if (!IsWalkable(Hit))
			{
				Normal = Normal.GetSafeNormal2D();
			}
		}
		else if (Normal.Z < -KINDA_SMALL_NUMBER)
		{
			// Don't push down into the floor when the impact is on the upper portion of the capsule.
			if (CurrentFloor.FloorDist < MIN_FLOOR_DIST && CurrentFloor.bBlockingHit)
			{
				const FVector FloorNormal = GetOrientationComp()->TransformDirectionW2L(CurrentFloor.HitResult.Normal); //Changed
				const bool bFloorOpposedToMovement = (GetOrientationComp()->TransformDirectionW2L(Delta) | FloorNormal) < 0.f && (FloorNormal.Z < 1.f - DELTA);
				if (bFloorOpposedToMovement)
				{
					Normal = FloorNormal;
				}

				Normal = Normal.GetSafeNormal2D();
			}
		}
	}

	//Since we are replacing the Super function we need to ignore it
	return Super::Super::SlideAlongSurface(Delta, Time, GetOrientationComp()->TransformDirectionL2W(Normal), Hit, bHandleImpact); //Changed
}

void UAdvKitGravityCharacterMovementComponent::PhysicsRotation(float DeltaSeconds)
{
	//Without an orientation component use default implementation
	if (!GetOrientationComp())
	{
		Super::PhysicsRotation(DeltaSeconds);
		return;
	}

	if (!HasValidData() || !CharacterOwner->Controller)
	{
		return;
	}

	bool bInTransition = AdvKitCharacterOwner->GetActiveAction() && AdvKitCharacterOwner->GetActiveAction()->IsA(UAdvKitCA_ZoneTransition::StaticClass());
	if (bInTransition)
	{
		return;
	}

	const FRotator CurrentRotation = CharacterOwner->GetActorRotation();
	FRotator DeltaRot = GetDeltaRotation(DeltaSeconds);
	FRotator DesiredRotation = CurrentRotation;

	// Accumulate a desired new rotation.
	FRotator NewRotation = CurrentRotation;
	FVector DeltaMove = FVector::ZeroVector;

	switch (GetAdvMovementMode())
	{
	case EAdvKitMovementMode::None:
	case EAdvKitMovementMode::Walking:
	case EAdvKitMovementMode::Falling:
	case EAdvKitMovementMode::ClimbingCeiling:
		if (bOrientRotationToMovement)
		{
			DesiredRotation = ComputeOrientToMovementRotation(CurrentRotation, DeltaSeconds, DeltaRot);
		}
		else if (bUseControllerDesiredRotation)
		{
			DesiredRotation = CharacterOwner->Controller->GetDesiredRotation();
		}
		else
		{
			return;
		}


		// Always remain vertical when walking or falling.
		if (IsMovingOnGround() || IsFalling())
		{
			if (GetOrientationComp())
			{
				DesiredRotation = GetOrientationComp()->TransformRotationW2L(DesiredRotation); //Changed
				DesiredRotation.Pitch = 0;
				DesiredRotation.Roll = 0;
				DesiredRotation = GetOrientationComp()->TransformRotationL2W(DesiredRotation); //Changed
			}
			else
			{
				DesiredRotation.Pitch = 0;
				DesiredRotation.Roll = 0;
			}
		}

		if (CurrentRotation.GetDenormalized().Equals(DesiredRotation.GetDenormalized(), 0.01f))
		{
			return;
		}


		/** TODO //Changed
		//YAW
		if (DesiredRotation.Yaw != CurrentRotation.Yaw)
		{
		NewRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
		}

		// PITCH
		if (DesiredRotation.Pitch != CurrentRotation.Pitch)
		{
		NewRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
		}

		// ROLL
		if (DesiredRotation.Roll != CurrentRotation.Roll)
		{
		NewRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
		}
		*/
		NewRotation = FQuat::Slerp(CurrentRotation.Quaternion(), DesiredRotation.Quaternion(), DeltaSeconds*10.f).Rotator(); //TODO rotate at constant rate  //Changed

		//UpdatedComponent->AngularVelocity = CharAngularVelocity( CurrentRotation, NewRotation, DeltaSeconds );
		break;
	case EAdvKitMovementMode::Flying:
		break;
	case EAdvKitMovementMode::Swimming:
		break;
	//These zones all have fixed orientations for the character
	case EAdvKitMovementMode::ClimbingWall:
	case EAdvKitMovementMode::WalkingTightspace:
	case EAdvKitMovementMode::ClimbingLedge:
	case EAdvKitMovementMode::ClimbingLadder:
	case EAdvKitMovementMode::WalkingBalance:
		NewRotation = GetDesiredRotationInZone(GetZone(),GetZoneLocation());
		break;
	case EAdvKitMovementMode::Custom:
		break;
	default:
		//UE_LOG(LogAdvKitGravityCharacterMovement, Warning, TEXT("%s has unsupported movement mode %d"), *CharacterOwner->GetName(), int32(MovementMode));
		//SetMovementMode(MOVE_None);
		break;
	}

	//TODO
	//UpdatedComponent->AngularVelocity = CharAngularVelocity( CurrentRotation, NewRotation, DeltaSeconds );

	// Set the new rotation.
	if (!NewRotation.Equals(CurrentRotation.GetDenormalized(), 0.01f))
	{
		MoveUpdatedComponent(DeltaMove, NewRotation, true);
	}
}

