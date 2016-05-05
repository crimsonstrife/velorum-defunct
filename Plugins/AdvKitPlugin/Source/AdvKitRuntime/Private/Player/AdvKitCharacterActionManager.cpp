

#include "AdvKitRuntime.h"
#include "Player/AdvKitCharacterActionManager.h"
#include "Player/AdvKitCharacter.h"
#include "Actions/AdvKitCharacterAction.h"
#include "Net/UnrealNetwork.h"


AAdvKitCharacterActionManager::AAdvKitCharacterActionManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	bReplicateMovement = false;

	bLogDebugInfo = false;
}

void AAdvKitCharacterActionManager::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAdvKitCharacterActionManager, CurrentAction);
	DOREPLIFETIME(AAdvKitCharacterActionManager, Actions);
}

void AAdvKitCharacterActionManager::BeginPlay()
{
	Super::BeginPlay();

	//if (Role < ROLE_Authority)
	//{
	//	return;
	//}

	AAdvKitCharacter* CharacterOwner = Cast<AAdvKitCharacter>(GetOwner());
	//Useless to manage actions on a non advkit character
	check(CharacterOwner);

	//TODO this breaks multiplayer (Checksum failed error)

	//Create all configured actions of the character
	for (TSubclassOf<class UAdvKitCharacterAction> ActionClass : CharacterOwner->DefaultActions)
	{
		//Configured entry might be empty
		if (!IsValid(ActionClass))
		{
			continue;
		}

		//Create action
		UAdvKitCharacterAction* NewAction = NewObject<UAdvKitCharacterAction>(this, ActionClass);
		if (!IsValid(NewAction))
		{
			continue;
		}

		//Add action and create subsequent actions
		AddAction(NewAction);
	}
}

void AAdvKitCharacterActionManager::EndPlay(EEndPlayReason::Type Reason)
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	CurrentAction = NULL;

	//Remove all actions
	for (UAdvKitCharacterAction* Action : Actions)
	{
		Action->DestroyComponent();
	}

	Actions.Empty();

	Super::EndPlay(Reason);
}

void AAdvKitCharacterActionManager::AddAction(class UAdvKitCharacterAction* Action)
{
	//Don't add null objects
	if (!IsValid(Action))
	{
		return;
	}

	//Init action and setup
	Action->Initialize(Cast<AAdvKitCharacter>(Instigator));
	Action->RegisterComponent();
	Action->SetIsReplicated(true);
	Actions.AddUnique(Action);

	//If the action has need of subsequent actions, add them, too
	auto AdditionalActionClasses = Action->GetAdditionalRequiredActions();
	for (auto AdditionalClass : AdditionalActionClasses)
	{
		//Disregard empty values
		if (!AdditionalClass)
		{
			continue;
		}

		//Create and add new action
		UAdvKitCharacterAction* NewAction = NewObject<UAdvKitCharacterAction>(this, AdditionalClass);
		if (NewAction)
		{
			AddAction(NewAction);
		}
	}
}

FAdvKitActionResponse AAdvKitCharacterActionManager::StartAction(TSubclassOf<class UAdvKitCharacterAction> ActionClass, class UAdvKitCharacterAction_Arguments* Arguments)
{
	//Response object, initially a failure unless changed later
	FAdvKitActionResponse Response;
	Response.Type = EAdvKitActionResponseType::Failure;

	//No class means it can't be started
	if (!ActionClass)
	{
		return Response;
	}

	//Find new action by class
	UAdvKitCharacterAction* NewAction = NULL;
	for (UAdvKitCharacterAction* TempAction : Actions)
	{
		if (TempAction->IsA(ActionClass))
		{
			NewAction = TempAction;
			break;
		}
	}

	//Desired action is not present in the configuration
	if (NewAction == NULL)
	{
		LogDebug(ELogVerbosity::Warning, "AAdvKitCharacterActionManager::StartAction Action not found", ActionClass);
		Response.Type = EAdvKitActionResponseType::NotFound;
		return Response;
	}

	//Call server if action was started by client
	if (Role < ROLE_Authority)
	{
		StartActionServer(ActionClass, Arguments);
	}

	LogDebug(ELogVerbosity::Log, "AAdvKitCharacterActionManager::StartAction - Trying to start ", ActionClass);

	//Check if the new action would interrupt an ongoing one
	UAdvKitCharacterAction* InterruptedAction = NULL;
	if (IsCurrentActionStillActive())
	{
		LogDebug(ELogVerbosity::Log, "AAdvKitCharacterActionManager::StartAction - Current action has not ended");

		//If the active action can be interrupted, do it
		if (CurrentAction->CanBeInterruptedBy(NewAction) || NewAction->CanInterrupt(CurrentAction))
		{
			CurrentAction->Stop(NewAction);
			InterruptedAction = CurrentAction;
		}
		else
		{
			//Otherwise don't start the new action
			LogDebug(ELogVerbosity::Warning, "AAdvKitCharacterActionManager::StartAction - Current action cannot be interrupted", CurrentAction->GetClass());
			return Response;
		}
	}
	else
	{
		LogDebug(ELogVerbosity::Log, "AAdvKitCharacterActionManager::StartAction - No active action");
	}

	//Start the new action
	LogDebug(ELogVerbosity::Log, "AAdvKitCharacterActionManager::StartAction - Starting action", ActionClass);
	CurrentAction = NewAction;
	Response = CurrentAction->Start(Arguments, InterruptedAction);

	//Start of the action resulted in a redirect, execute the redirect
	if (Response.IsRedirect())
	{
		LogDebug(ELogVerbosity::Log, "AAdvKitCharacterActionManager::StartAction - Redirected to action", Response.RedirectAction);

		//Return result of redirect start
		return StartAction(Response.RedirectAction, Response.RedirectArguments);
	}

	//Could not start action
	if (Response.IsFailure())
	{
		LogDebug(ELogVerbosity::Warning, "AAdvKitCharacterActionManager::StartAction - Action failed to start", ActionClass);
	}
	else
	{
		LogDebug(ELogVerbosity::Log, "AAdvKitCharacterActionManager::StartAction - Action started", ActionClass);
	}

	//Return result
	return Response;
}

UAdvKitCharacterAction* AAdvKitCharacterActionManager::GetCurrentAction() const
{
	return CurrentAction;
}

UAdvKitCharacterAction* AAdvKitCharacterActionManager::GetActiveAction() const
{
	if (!IsCurrentActionStillActive())
	{
		return nullptr;
	}

	return CurrentAction;
}


bool AAdvKitCharacterActionManager::IsCurrentActionStillActive() const
{
	if (!CurrentAction)
	{
		return false;
	}

	return CurrentAction->IsActive();
}

void AAdvKitCharacterActionManager::StartActionServer_Implementation(TSubclassOf<class UAdvKitCharacterAction> ActionClass, class UAdvKitCharacterAction_Arguments* Arguments)
{
	StartAction(ActionClass, Arguments);
}

bool AAdvKitCharacterActionManager::StartActionServer_Validate(TSubclassOf<class UAdvKitCharacterAction> ActionClass, class UAdvKitCharacterAction_Arguments* Arguments)
{
	return true;
}

void AAdvKitCharacterActionManager::LogDebug(ELogVerbosity::Type LocalLogLevel, const FString& Message, const TSubclassOf<UAdvKitCharacterAction> ActionClass)
{
	if (!bLogDebugInfo)
	{
		return;
	}

	FString ActionClassName = "None";
	if (IsValid(ActionClass))
	{
		ActionClassName = ActionClass->GetName();
	}

	FString ActualMessage = Message + "(" + ActionClassName + ")";
	switch (LocalLogLevel)
	{
		UE_LOG(LogAdvKit, Log, TEXT("%s"), *ActualMessage);

	case ELogVerbosity::Log:
		UE_LOG(LogAdvKit, Log, TEXT("%s"), *ActualMessage);
		break;
	case ELogVerbosity::Warning:
		UE_LOG(LogAdvKit, Warning, TEXT("%s"), *ActualMessage);
		break;
	case ELogVerbosity::Error:
		UE_LOG(LogAdvKit, Error, TEXT("%s"), *ActualMessage);
		break;
	}
}
