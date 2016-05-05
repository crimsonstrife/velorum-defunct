// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/Movement/AdvKitCA_PhysicsTransition.h"
#include "Actions/Movement/AdvKitCA_PhysicsTransition_Arguments.h"

#include "Player/AdvKitCharacter.h"


UAdvKitCA_PhysicsTransition::UAdvKitCA_PhysicsTransition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}

void UAdvKitCA_PhysicsTransition::InitializeComponent()
{
	Super::InitializeComponent();
	
	//Create transition map based upon configuration
	for (FAdvKitPhysicsTransition& Transition : DefaultTransitionActions)
	{
		AddTransiton(Transition.FromMode, Transition.ToMode, Transition.TransitionAction);
		if (Transition.FromMode != Transition.ToMode)
		{
			AddTransiton(Transition.ToMode, Transition.FromMode, Transition.ReverseAction);
		}
	}
}

FAdvKitActionResponse UAdvKitCA_PhysicsTransition::Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments, UAdvKitCharacterAction* InterruptedOther)
{
	//Cannot work without parameters
	UAdvKitCA_PhysicsTransition_Arguments* TransitionArguments = Cast<UAdvKitCA_PhysicsTransition_Arguments>(Arguments);
	if (!TransitionArguments)
	{
#if WITH_EDITOR
		if (bLogDebug)
		{
			UE_LOG(LogAdvKit, Warning, TEXT("Physics Transition has no arguments"));
		}
#endif
		return Failed();
	}


#if WITH_EDITOR
	if (bLogDebug)
	{
		UE_LOG(LogAdvKit, Warning, TEXT("Transition %s to %s"),
			*GetMovementModeDisplayName(TransitionArguments->FromMode),
			*GetMovementModeDisplayName(TransitionArguments->ToMode));
	}
#endif

	//Check if there are transitions from this mode
	if (!TransitionMap.Contains((uint8)TransitionArguments->FromMode))
	{
#if WITH_EDITOR
		if (bLogDebug)
		{
			UE_LOG(LogAdvKit, Warning, TEXT("Transition %s to %s failed because From could not be found"),
				*GetMovementModeDisplayName(TransitionArguments->FromMode),
				*GetMovementModeDisplayName(TransitionArguments->ToMode));
		}
#endif
		return Failed();
	}

	const TMap<uint8, TSubclassOf<UAdvKitCharacterAction> >& ToMap = TransitionMap[(uint8)TransitionArguments->FromMode];

	//Check if the transition map from this mode has a transition for the ToMode
	if (!ToMap.Contains((uint8)TransitionArguments->ToMode))
	{
#if WITH_EDITOR
		if (bLogDebug)
		{
			UE_LOG(LogAdvKit, Warning, TEXT("Transition %s to %s failed because To could not be found"),
				*GetMovementModeDisplayName(TransitionArguments->FromMode),
				*GetMovementModeDisplayName(TransitionArguments->ToMode));
		}
#endif
		return Failed();
	}

	//Redirect according to map
	return Redirect(ToMap[(uint8)TransitionArguments->ToMode], TransitionArguments->SubArguments);
}

FAdvKitActionResponse UAdvKitCA_PhysicsTransition::Stop_Implementation(const UAdvKitCharacterAction* InterruptedBy)
{
	return Succeeded();
}

bool UAdvKitCA_PhysicsTransition::CanBeInterruptedBy_Implementation(const UAdvKitCharacterAction* Other) const
{
	return false;
}

UAdvKitCharacterAction_Arguments* UAdvKitCA_PhysicsTransition::MakeArguments(EAdvKitMovementMode From, EAdvKitMovementMode To, UAdvKitCharacterAction_Arguments* SubArguments)
{
	UAdvKitCA_PhysicsTransition_Arguments* Arguments = NewObject<UAdvKitCA_PhysicsTransition_Arguments>();
	Arguments->FromMode = From;
	Arguments->ToMode = To;
	Arguments->SubArguments = SubArguments;
	return Arguments;
}

void UAdvKitCA_PhysicsTransition::AddTransiton(EAdvKitMovementMode FromMode, EAdvKitMovementMode ToMode, TSubclassOf<UAdvKitCharacterAction> TransitionAction)
{
	if (!TransitionAction)
	{
		//No action defined
#if WITH_EDITOR
		UE_LOG(LogAdvKit, Warning, TEXT("UAdvKitCA_PhysicsTransition::PostInitializeComponents No action defined for transition %s to %s"),
			*GetMovementModeDisplayName(FromMode),
			*GetMovementModeDisplayName(ToMode));
#endif
		return;
	}

	TMap<uint8, TSubclassOf<UAdvKitCharacterAction> >& ToMap = TransitionMap.FindOrAdd((uint8)FromMode);
	if (ToMap.Contains((uint8)ToMode))
	{
		//Mapping already in use
#if WITH_EDITOR
		UE_LOG(LogAdvKit, Warning, TEXT("UAdvKitCA_PhysicsTransition::PostInitializeComponents Double to-key %s for action %s"),
			*GetMovementModeDisplayName(ToMode),
			*TransitionAction->GetName());
#endif
		return;
	}

	ToMap.Add((uint8)ToMode, TransitionAction);
}

TArray<TSubclassOf<UAdvKitCharacterAction>> UAdvKitCA_PhysicsTransition::GetAdditionalRequiredActions_Implementation()
{
	TArray<TSubclassOf<UAdvKitCharacterAction>> AdditionalActions;
	
	//Add all configured actions
	for (FAdvKitPhysicsTransition& Transition : DefaultTransitionActions)
	{
		AdditionalActions.AddUnique(Transition.TransitionAction);
		AdditionalActions.AddUnique(Transition.ReverseAction);
	}

	return AdditionalActions;
}

