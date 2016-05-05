// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/AdvKitCharacterAction.h"
#include "Player/AdvKitCharacter.h"
#include "Player/AdvKitInventoryManager.h"

#include "Actions/AdvKitCharacterAction_StateConfiguration.h"
#include "Actions/AdvKitCharacterAction_CharacterModifier.h"

#include "Net/UnrealNetwork.h"

bool FAdvKitActionResponse::IsSuccess()
{
	return Type == EAdvKitActionResponseType::Success;
}

bool FAdvKitActionResponse::IsFailure()
{
	return Type == EAdvKitActionResponseType::Failure;
}

bool FAdvKitActionResponse::IsRedirect()
{
	return Type == EAdvKitActionResponseType::Redirect;
}

bool FAdvKitActionResponse::IsNotFound()
{
	return Type == EAdvKitActionResponseType::NotFound;
}

UAdvKitCharacterAction::UAdvKitCharacterAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAutoActivate = false;
}

void UAdvKitCharacterAction::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAdvKitCharacterAction, CharacterOwner);
	DOREPLIFETIME(UAdvKitCharacterAction, MovementComponent);
}

void UAdvKitCharacterAction::Initialize(AAdvKitCharacter *NewCharacterOwner)
{
	check(NewCharacterOwner);
	CharacterOwner = NewCharacterOwner;
	MovementComponent = Cast<UAdvKitCharacterMovementComponent>(CharacterOwner->GetMovementComponent());
}

void UAdvKitCharacterAction::PlayAnimMontage(UAnimMontage *Montage)
{
	if (!CharacterOwner)
		return;

	CharacterOwner->PlayAnimMontage(Montage);

	if (!CharacterOwner->GetMesh())
		return;
	
	if (!CharacterOwner->GetMesh()->GetAnimInstance())
		return;

	OnMontageEndedDelegate.BindUObject(this, &UAdvKitCharacterAction::OnMontageEnded);
	CharacterOwner->GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(OnMontageEndedDelegate, Montage);
}

void UAdvKitCharacterAction::OnMontageEnded_Implementation(UAnimMontage *EndedMontage, bool bInterrupted)
{
}

FAdvKitActionResponse UAdvKitCharacterAction::Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments, UAdvKitCharacterAction* InterruptedOther)
{
	SetActive(true);
	ApplyStartModifiers();

	return Succeeded();
}

FAdvKitActionResponse UAdvKitCharacterAction::Stop_Implementation(const UAdvKitCharacterAction* InterruptedBy)
{
	ApplyStopModifiers();
	OnMontageEndedDelegate.Unbind();
	SetActive(false);
	return Succeeded();
}

bool UAdvKitCharacterAction::CanBeInterruptedBy_Implementation(const UAdvKitCharacterAction* Other) const
{
	//Dummy implementation to be overridden in subclass
	return true;
}

bool UAdvKitCharacterAction::CanInterrupt_Implementation(const UAdvKitCharacterAction* Other) const
{
	//Dummy implementation to be overridden in subclass
	return false;
}

FAdvKitActionResponse UAdvKitCharacterAction::Redirect(TSubclassOf<class UAdvKitCharacterAction> ToAction, UAdvKitCharacterAction_Arguments* WithArguments) 
{
	FAdvKitActionResponse NewResponse;
	NewResponse.Type = EAdvKitActionResponseType::Redirect;
	NewResponse.RedirectAction = ToAction;
	NewResponse.RedirectArguments = WithArguments;

	return NewResponse;
}

FAdvKitActionResponse UAdvKitCharacterAction::Succeeded() 
{
	FAdvKitActionResponse NewResponse;
	NewResponse.Type = EAdvKitActionResponseType::Success;

	return NewResponse;
}

FAdvKitActionResponse UAdvKitCharacterAction::Failed() 
{
	FAdvKitActionResponse NewResponse;
	NewResponse.Type = EAdvKitActionResponseType::Failure;

	return NewResponse;
}

TArray<TSubclassOf<UAdvKitCharacterAction>> UAdvKitCharacterAction::GetAdditionalRequiredActions_Implementation()
{
	//Dummy implementation to be overridden in subclass
	return TArray<TSubclassOf<UAdvKitCharacterAction>>();
}

void UAdvKitCharacterAction::ApplyStartModifiers()
{
	//ConfigOnStart->ApplyToCharacter(CharacterOwner, MovementComponent);
	for (auto Config : StartModifiers)
	{
		if (!IsValid(Config))
		{
			continue;
		}

		Config->ApplyToCharacter(CharacterOwner, MovementComponent);
	}
}


void UAdvKitCharacterAction::ApplyStopModifiers()
{
	//ConfigOnStop->ApplyToCharacter(CharacterOwner, MovementComponent);
	for (auto Config : StopModifiers)
	{
		if (!IsValid(Config))
		{
			continue;
		}

		Config->ApplyToCharacter(CharacterOwner, MovementComponent);
	}
}

AAdvKitCharacter* UAdvKitCharacterAction::GetCharacterOwner()
{
	return CharacterOwner;
}

UAdvKitCharacterMovementComponent* UAdvKitCharacterAction::GetMovementComponent()
{
	return MovementComponent;
}
