// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Player/AdvKitFootstepComponent.h"


UAdvKitFootstepComponent::UAdvKitFootstepComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Add default socket names
	FootSocketNames.Add(FName(*FString("foot_l")));
	FootSocketNames.Add(FName(*FString("foot_r")));

	FootStepTraceDistance = 50.0f;
}

void UAdvKitFootstepComponent::PlayFootstep_Implementation(int32 Foot)
{
	InternalPlayFootstep(Foot, false);
}

UAudioComponent* UAdvKitFootstepComponent::PlayFootstepAttached_Implementation(int32 Foot)
{
	return InternalPlayFootstep(Foot, true);
}

UAudioComponent* UAdvKitFootstepComponent::InternalPlayFootstep(int32 Foot, bool bAttached)
{
	//Component works only with characters
	auto Character = Cast<ACharacter>(GetOwner());
	if (!Character)
	{
		return nullptr;
	}

	//Need mesh to for socket transforms
	auto Mesh = Character->GetMesh();
	if (!Mesh)
	{
		return nullptr;
	}

	//Foot index needs to be 
	if (Foot >= FootSocketNames.Num())
	{
		return nullptr;
	}

	//Get foot socket location
	FVector FootLocation = Mesh->GetSocketLocation(FootSocketNames[Foot]);

	//Configure parameters
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);
	Params.bReturnPhysicalMaterial = true;
	FCollisionObjectQueryParams ObjectParams;

	//Try to hit a surface below the foot
	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByObjectType(Hit, FootLocation, FootLocation - Character->GetActorUpVector()*FootStepTraceDistance, ObjectParams, Params))
	{
		return nullptr;
	}

	//If there is no phys material there can be no surface
	if (!Hit.PhysMaterial.IsValid())
	{
		return nullptr;
	}

	//Iterate through configs
	EPhysicalSurface HitSurface = Hit.PhysMaterial->SurfaceType;
	for (auto& FootStepSound : FootStepSounds)
	{
		//Ignore configs that do not match surface
		if (FootStepSound.Surface != HitSurface)
		{
			continue;
		}

		//Play sound
		if (bAttached)
		{
			return UGameplayStatics::SpawnSoundAttached(FootStepSound.Sound, Character->GetRootComponent(), NAME_None, FootLocation, EAttachLocation::SnapToTarget);
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(this, FootStepSound.Sound, FootLocation);
			return nullptr;
		}
	}

	return nullptr;
}
