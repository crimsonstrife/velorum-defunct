// Copyright 2015 Pascal Krabbe

#pragma once

#include "Actions/AdvKitCharacterAction.h"
#include "AdvKitCharacterActionManager.generated.h"

class UAdvKitCharacterAction;

/**
 * @brief Manager class for actions of the owning character. It takes care of instantiation and actiavtion.
 */
UCLASS(/*Within = AdvKitCharacter*/)
class ADVKITRUNTIME_API AAdvKitCharacterActionManager : public AActor
{
	GENERATED_BODY()

protected:
	/** List holding all actions that are currently handled by the manager. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions", Replicated)
	TArray<UAdvKitCharacterAction*> Actions;

	/** The currently active action. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions", Replicated)
	UAdvKitCharacterAction* CurrentAction;

	/** Flag to make action manager log debug messages when executing actions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actions")
	bool bLogDebugInfo;

public:
	/**
	 * Constructor
	 */
	AAdvKitCharacterActionManager(const FObjectInitializer& ObjectInitializer);

	/**
	 * Adds a new action to the manager and instantiates all additional actions needed
	 * @param	Action	The action to add
	 */
	virtual void AddAction(UAdvKitCharacterAction* Action);

	/** Tries to start a new character action on the owning character
	 * @param	ActionClass		Class of the action to start
	 * @param	Arguments	Arguments passed to the started action
	 * @return	Response of the started action
	 */
	virtual FAdvKitActionResponse StartAction(TSubclassOf<UAdvKitCharacterAction> ActionClass, UAdvKitCharacterAction_Arguments* Arguments = nullptr);


	/**
	 * Getter for the currently set action
	 * @return CurrentAction 
	 */
	UAdvKitCharacterAction* GetCurrentAction() const;

	/**
	 * Getter for the currently active action
	 * @return CurrentAction if still active
	 */
	UAdvKitCharacterAction* GetActiveAction() const;

	/**
	 * Check if the current action is still being executed
	 * @return false if there is no action or the action is no longer active
	 */
	bool IsCurrentActionStillActive() const;

	//Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason);
	//End AActor Interface

protected:

	/** Server callback to start an action
	 * @param	ActionClass		Class of the action to start
	 * @param	Arguments	Arguments passed to the started action
	 */
	UFUNCTION(Reliable, Server, WithValidation)
	void StartActionServer(TSubclassOf<UAdvKitCharacterAction> ActionClass, UAdvKitCharacterAction_Arguments* Arguments = nullptr);
	virtual void StartActionServer_Implementation(TSubclassOf<class UAdvKitCharacterAction> ActionClass, class UAdvKitCharacterAction_Arguments* Arguments = NULL);


	/**
	 * Internal debug method to log messages in the form of "[LogLevel] : Message (ActionName)"
	 * @param	LogLevel 	Log level of the message
	 * @param	Message 	The message to log
	 * @param	ActionClass 	Action class that caused the message / is important for debug
	 */
	void LogDebug(ELogVerbosity::Type LocalLogLevel, const FString& Message, const TSubclassOf<UAdvKitCharacterAction> ActionClass = nullptr);
};
