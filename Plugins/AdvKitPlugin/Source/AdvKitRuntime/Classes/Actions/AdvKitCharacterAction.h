// Copyright 2015 Pascal Krabbe

#pragma once

#include "Components/ActorComponent.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "Actions/AdvKitCharacterAction_Arguments.h"
#include "AdvKitCharacterAction.generated.h"

/**
 * @brief Enumeration of possible response states for actions
 */
UENUM(BlueprintType)
enum class EAdvKitActionResponseType : uint8
{
	Success,
	Failure,
	Redirect,
	NotFound
};

/**
 * @brief Response struct when trying to start an action. 
 */
USTRUCT(BlueprintType)
struct ADVKITRUNTIME_API FAdvKitActionResponse
{
	GENERATED_USTRUCT_BODY()

	/** Type of response */
	UPROPERTY(BlueprintReadOnly)
	EAdvKitActionResponseType Type;

	/** In case response is a redirect, this is the target action*/
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<class UAdvKitCharacterAction> RedirectAction;

	/** Arguments for the redirect action */
	UPROPERTY(BlueprintReadOnly)
	UAdvKitCharacterAction_Arguments* RedirectArguments;

	/**
	 * Utility to check if response is a success
	 * @return True if success
	 */
	virtual bool IsSuccess();

	/**
	 * Utility to check if response is a failure
	 * @return True if failure
	 */
	virtual bool IsFailure();

	/**
	 * Utility to check if response is a redirect
	 * @return True if redirect
	 */
	virtual bool IsRedirect();

	/**
	 * Utility to check if action was not found
	 * @return True if not found
	 */
	virtual bool IsNotFound();

	/** Constructor */
	FAdvKitActionResponse()
	{
		Type = EAdvKitActionResponseType::Failure;
		RedirectAction = nullptr;
		RedirectArguments = nullptr;
	}
};

/**
 * @brief A character action makes its owning character execute a certain set of configurations.
 * It can change certain variables of the character when started and/or when ending.
 */
UCLASS(Blueprintable, BlueprintType, abstract/*, Within = AdvKitCharacterActionManager*/)
class ADVKITRUNTIME_API UAdvKitCharacterAction : public UActorComponent 
{
	GENERATED_BODY()
protected:
	/** Configurations to set the CharacterOwner to when the action starts */
	UPROPERTY(Instanced, EditDefaultsOnly, BlueprintReadOnly, Category = "Action", meta = (Tooltip = "Configurations for the character once the action starts."))
	TArray<class UAdvKitCharacterAction_CharacterModifier*> StartModifiers;

	/** Configurations to set the CharacterOwner to when the action starts */
	UPROPERTY(Instanced, EditDefaultsOnly, BlueprintReadOnly, Category = "Action", meta = (Tooltip = "Configurations for the character once the action stops."))
	TArray<class UAdvKitCharacterAction_CharacterModifier*> StopModifiers;

	/** Character that is executing this action */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Action", Replicated)
	class AAdvKitCharacter* CharacterOwner;

	/** MovementComponent of the CharacterOwner */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Action", Replicated)
	class UAdvKitCharacterMovementComponent* MovementComponent;

protected:
	/** Plays an AnimMontage on the CharacterOwner. */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual void PlayAnimMontage(UAnimMontage *Montage);

	/** Delegate that gets registered on played AnimMontages and calls OnMontageEnded */
	FOnMontageEnded OnMontageEndedDelegate;

	/** Called when a montage played on the CharacterOwner has ended */
	UFUNCTION(BlueprintNativeEvent, Category="AdvKit")
	void OnMontageEnded(UAnimMontage *EndedMontage, bool bInterrupted);
	virtual void OnMontageEnded_Implementation(UAnimMontage *EndedMontage, bool bInterrupted);

public:
	/**
	 * Constructor
	 */
	UAdvKitCharacterAction(const FObjectInitializer& ObjectInitializer);

	/**
	 * Initializes the action for a character
	 * @param	NewCharacterOwner	The character for that will execute this action.
	 */
	virtual void Initialize(AAdvKitCharacter *NewCharacterOwner);

	/**
	 * Starts the action.
	 * @param	Arguments	Optional arguments for the action.
	 * @param	InterruptedOther	Optional action that was interrupted when starting this action
	 * @return	Response containing more information about the state of the action.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AdvKit")
	FAdvKitActionResponse Start(class UAdvKitCharacterAction_Arguments* Arguments = NULL, UAdvKitCharacterAction* InterruptedOther = NULL);
	virtual FAdvKitActionResponse Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments = NULL, UAdvKitCharacterAction* InterruptedOther = NULL);

	/**
	 * Stops the action
	 * @note Every action can potentially be interrupted, an action cannot revoke interrupts.
	 * @param	InterruptedBy	(Optional) The action that caused to stop this action prematurely.
	 * @return	Response containing more information about the state of the action.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AdvKit")
	FAdvKitActionResponse Stop(const UAdvKitCharacterAction* InterruptedBy = NULL);
	virtual FAdvKitActionResponse Stop_Implementation(const UAdvKitCharacterAction* InterruptedBy = NULL);

	/**
	 * Can this action be interrupted by another?
	 * @note This returning false does not make an action uninterruptible, since another action can
	 * return true in its CanInterrupt method. This is for easy extendability of action combinations
	 * @param	Other	The action that might be able to interrupt this action.
	 * @return			true in case this action can be interrupted, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "AdvKit")
	bool CanBeInterruptedBy(const UAdvKitCharacterAction* Other) const;
	virtual bool CanBeInterruptedBy_Implementation(const UAdvKitCharacterAction* Other) const;

	/**
	 * Can this action interrupt another action?
	 * @param	Other	The action that might be interruptible
	 * @return			true in case the other action can be interrupted, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "AdvKit")
	bool CanInterrupt(const UAdvKitCharacterAction* Other) const;
	virtual bool CanInterrupt_Implementation(const UAdvKitCharacterAction* Other) const;

	/**
	 * Redirect to another action
	 * @param	ToAction		Action class to redirect to
	 * @param	WithArguments	Arguments to pass to the redirected action
	 * @return	Response struct with redirection 
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	static FAdvKitActionResponse Redirect(TSubclassOf<class UAdvKitCharacterAction> ToAction, UAdvKitCharacterAction_Arguments* WithArguments = NULL);

	/**
	 * Returns a success response
	 * @return	Response struct with success flags
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	static FAdvKitActionResponse Succeeded();

	/**
	 * Returns a fail response
	 * @return	Response struct with fail flags
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	static FAdvKitActionResponse Failed();

	/**
	 * If this action requires sub actions to work, it can return the required actions as classes to
	 * be instantiated by the action manager.
	 * @returns	List of required actions
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "AdvKit")
	TArray<TSubclassOf<UAdvKitCharacterAction>> GetAdditionalRequiredActions();
	virtual TArray<TSubclassOf<UAdvKitCharacterAction>> GetAdditionalRequiredActions_Implementation();

	/**
	 * Applies set of start modifiers to the character
	 */
	void ApplyStartModifiers();

	/**
	 * Applies set of stop modifiers to the character
	 */
	void ApplyStopModifiers();

	/** @return	Character that is executing this action */
	class AAdvKitCharacter* GetCharacterOwner();

	/** @return	MovementComponent of the CharacterOwner */
	class UAdvKitCharacterMovementComponent* GetMovementComponent();

};
