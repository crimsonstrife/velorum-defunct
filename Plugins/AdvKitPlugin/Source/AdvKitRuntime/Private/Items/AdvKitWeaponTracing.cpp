// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Items/AdvKitWeaponTracing.h"
#include "Items/AdvKitFireModeConfiguration.h"
#include "Player/AdvKitTraceUtilityComponent.h"
#include "Player/AdvKitCharacter.h"

AAdvKitWeaponTracing::AAdvKitWeaponTracing(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TraceComponent = ObjectInitializer.CreateDefaultSubobject<UAdvKitTraceUtilityComponent>(this, TEXT("Trace Component"));
	TraceComponent->AttachTo(RootComponent);

	UnitsPerTrace = 10;
}

void AAdvKitWeaponTracing::StartTracing()
{
	//Clear hit list
	DamageStack.Empty();

	//Update values so sub traces do not start from where the weapon was disabled
	LastTraceStartPosition = TraceComponent->GetStart();
	LastTraceEndPosition = TraceComponent->GetEnd();

	LastTraceComponentLocation = TraceComponent->GetComponentLocation();
	LastTraceComponentRotation = TraceComponent->GetComponentQuat();

	bIsTracing = true;
}

void AAdvKitWeaponTracing::StopTracing()
{
	bIsTracing = false;
}
void AAdvKitWeaponTracing::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Weapon is not active, so no trace updates are needed
	if (!bIsTracing)
	{
		return;
	}

	//Cannot trace without substeps
	if (UnitsPerTrace == 0)
	{
		return;
	}

	//Calculate movement deltas
	FVector CurrentTraceStart = TraceComponent->GetStart();
	FVector CurrentTraceEnd = TraceComponent->GetEnd();

	FVector CurrentTraceComponentLocation = TraceComponent->GetComponentLocation();
	FQuat CurrentTraceComponentRotation = TraceComponent->GetComponentQuat();

	float MovedDistance = FMath::Max<float>(FVector::Dist(LastTraceStartPosition, CurrentTraceStart), FVector::Dist(LastTraceEndPosition, CurrentTraceEnd));

	//If the weapon has not moved no sub tracing is necessary
	if (MovedDistance <= 0)
	{
		return;
	}

	//Calculate the amount of steps for subtraces
	float Steps = FMath::Min<float>(1,MovedDistance / UnitsPerTrace);
	float StepSize = 1.0f / Steps;

	//Trace
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Instigator);
	Params.AddIgnoredActor(this);

	FCollisionObjectQueryParams ObjectParams;

	for (float Alpha = 0.0f; Alpha <= 1.0f; Alpha += StepSize)
	{
		FVector LerpedLocation = FMath::Lerp<FVector>(LastTraceComponentLocation, CurrentTraceComponentLocation, Alpha);
		FRotator LerpedRotation = FQuat::Slerp(LastTraceComponentRotation, CurrentTraceComponentRotation, Alpha).Rotator();
		
		TraceComponent->SetWorldLocation(LerpedLocation);
		TraceComponent->SetWorldRotation(LerpedRotation);
		TArray<FHitResult> Hits;
		
		if (TraceComponent->LineTraceMulti(Hits, Params, ObjectParams))
		{
			OnTraceHits(Hits);
		}
	}

	//Reset trace component location
	TraceComponent->SetWorldLocation(CurrentTraceComponentLocation);
	TraceComponent->SetWorldRotation(CurrentTraceComponentRotation.Rotator());

	LastTraceComponentLocation = CurrentTraceComponentLocation;
	LastTraceComponentRotation = CurrentTraceComponentRotation;
}

void AAdvKitWeaponTracing::OnTraceHits(const TArray<FHitResult>& Hits)
{
	//Avoid double hits
	for (FHitResult Hit : Hits)
	{
		if (DamageStack.Contains(Hit.Actor.Get()))
		{
			continue;
		}

		OnTraceHit(Hit);
	}

	//Can only hurt with fire mode config
	auto* ActiveFireMode = GetFireMode(WeaponState.ActiveFireMode);
	if (!ActiveFireMode)
	{
		return;
	}

	//Only cause damage on the server
	if (Role < ROLE_Authority)
	{
		return;
	}

	for (FHitResult Hit : Hits)
	{
		TryToApplyDamageToHit(Hit);
	}
}

void AAdvKitWeaponTracing::OnTraceHit_Implementation(FHitResult Hit)
{
	//Hook for hit effects
}

bool AAdvKitWeaponTracing::TryToApplyDamageToHit(FHitResult& Hit)
{
	//Don't apply damage more than once
	if (DamageStack.Contains(Hit.Actor.Get()))
	{
		return false;
	}

	//Can only hurt with fire mode config
	auto* ActiveFireMode = GetFireMode(WeaponState.ActiveFireMode);
	if (!ActiveFireMode)
	{
		return false;
	}

	//Cannot hit nothing
	if (Hit.Component.IsValid())
	{
		UGameplayStatics::ApplyPointDamage(Hit.Actor.Get(), ActiveFireMode->InstantDamage, -Hit.ImpactNormal, Hit, GetInstigatorController(), this, ActiveFireMode->InstantDamageTypeClass);
	}

	//Hit was damaged, cache to avoid dublicate hits
	FHitResult NewHit = FHitResult(Hit);
	DamageStack.Add(NewHit.Actor.Get(), NewHit);

	return true;
}
