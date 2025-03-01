#pragma once

#include "Engine.h"
#include "Core.h"
#include "OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineDelegateMacros.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemImpl.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystemUtilsModule.h"
#include "ModuleManager.h"
#include "OnlineSubsystemUtilsClasses.h"
#include "BlueprintDataDefinitions.generated.h"	


UENUM()
namespace ESessionSettingSearchResult
{
	enum Type
	{
		// Found the setting
		Found,

		// Did not find the setting
		NotFound,

		// Was not the correct ype
		WrongType
	};
}

// This makes a lot of the blueprint functions cleaner
UENUM()
namespace EBlueprintResultSwitch
{
	enum Type
	{
		// On Success
		OnSuccess,

		// On Failure
		OnFailure
	};
}

// This is to define server type searches
UENUM(BlueprintType)
namespace EBPServerPresenceSearchType
{
	enum Type
	{
		ClientServersOnly,
		DedicatedServersOnly
	};
}


// Wanted this to be switchable in the editor
UENUM(BlueprintType)
namespace EBPOnlinePresenceState
{
	enum Type
	{
		Online,
		Offline,
		Away,
		ExtendedAway,
		DoNotDisturb,
		Chat
	};
}

UENUM(BlueprintType)
namespace EBPOnlineSessionState
{
	enum Type
	{
		/** An online session has not been created yet */
		NoSession,
		/** An online session is in the process of being created */
		Creating,
		/** Session has been created but the session hasn't started (pre match lobby) */
		Pending,
		/** Session has been asked to start (may take time due to communication with backend) */
		Starting,
		/** The current session has started. Sessions with join in progress disabled are no longer joinable */
		InProgress,
		/** The session is still valid, but the session is no longer being played (post match lobby) */
		Ending,
		/** The session is closed and any stats committed */
		Ended,
		/** The session is being destroyed */
		Destroying
	};
}

// Boy oh boy is this a dirty hack, but I can't figure out a good way to do it otherwise at the moment
// The UniqueNetId is an abstract class so I can't exactly re-initialize it to make a shared pointer on some functions
// So I made the blueprintable UniqueNetID into a dual variable struct with access functions and I am converting the const var for the pointer
// I really need to re-think this later
USTRUCT(BlueprintType)
struct FBPUniqueNetId
{
	GENERATED_USTRUCT_BODY()

private:
	bool bUseDirectPointer;


public:
	TSharedPtr<const FUniqueNetId> UniqueNetId;
	const FUniqueNetId * UniqueNetIdPtr;

	void SetUniqueNetId(const TSharedPtr<const FUniqueNetId> &ID)
	{
		bUseDirectPointer = false;
		UniqueNetIdPtr = nullptr;
		UniqueNetId = ID;
	}

	void SetUniqueNetId(const FUniqueNetId *ID)
	{
		bUseDirectPointer = true;
		UniqueNetIdPtr = ID;
	}

	bool IsValid() const
	{
		if (bUseDirectPointer && UniqueNetIdPtr != nullptr)
		{
			return true;
		}
		else if (UniqueNetId.IsValid())
		{
			return true;
		}
		else
			return false;

	}

	const FUniqueNetId* GetUniqueNetId() const
	{
		if (bUseDirectPointer && UniqueNetIdPtr != nullptr)
		{
			// No longer converting to non const as all functions now pass const UniqueNetIds
			return /*const_cast<FUniqueNetId*>*/(UniqueNetIdPtr);
		}
		else if (UniqueNetId.IsValid())
		{
			return UniqueNetId.Get();
		}
		else
			return nullptr;
	}

	FBPUniqueNetId()
	{
		bUseDirectPointer = false;
		UniqueNetIdPtr = nullptr;
	}
};

USTRUCT(BluePrintType)
struct FBPOnlineUser
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FBPUniqueNetId UniqueNetId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString RealName;
};

USTRUCT(BluePrintType)
struct FBPOnlineRecentPlayer : public FBPOnlineUser
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString LastSeen;
};


USTRUCT(BlueprintType)
struct FBPFriendPresenceInfo
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		bool bIsOnline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		bool bIsPlaying;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		bool bIsPlayingThisGame;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
	bool bIsJoinable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		bool bHasVoiceSupport;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
	TEnumAsByte<EBPOnlinePresenceState::Type> PresenceState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString StatusString;
};



USTRUCT(BlueprintType)
struct FBPFriendInfo
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
	FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
	FString RealName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
	TEnumAsByte<EBPOnlinePresenceState::Type> OnlineState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
	FBPUniqueNetId UniqueNetId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
	bool bIsPlayingSameGame;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
	FBPFriendPresenceInfo PresenceInfo;
};

/** The types of comparison operations for a given search query */
// Used to compare session properties
UENUM(BlueprintType)
namespace EOnlineComparisonOpRedux
{
	enum Type
	{
		Equals,
		NotEquals,
		GreaterThan,
		GreaterThanEquals,
		LessThan,
		LessThanEquals,
	};
}


// Used to store session properties before converting to FVariantData
USTRUCT(BlueprintType)
struct FSessionPropertyKeyPair
{
	GENERATED_USTRUCT_BODY()

	FName Key;
	FVariantData Data;
};


// Sent to the FindSessionsAdvanced to filter the end results
USTRUCT(BlueprintType)
struct FSessionsSearchSetting
{
	GENERATED_USTRUCT_BODY()
	//UPROPERTY()


	// Had to make a copy of this to account for the original not being exposed to blueprints
	/** How is this session setting compared on the backend searches */
	TEnumAsByte<EOnlineComparisonOpRedux::Type> ComparisonOp;

	// The key pair to search for
	FSessionPropertyKeyPair PropertyKeyPair;
};

// Couldn't use the default one as it is not exposed to other modules, had to re-create it here
// Helper class for various methods to reduce the call hierarchy
struct FOnlineSubsystemBPCallHelperAdvanced
{
public:
	FOnlineSubsystemBPCallHelperAdvanced(const TCHAR* CallFunctionContext, UWorld* World, FName SystemName = NAME_None)
		: OnlineSub(Online::GetSubsystem(World, SystemName))
		, FunctionContext(CallFunctionContext)
	{
		if (OnlineSub == nullptr)
		{
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("%s - Invalid or uninitialized OnlineSubsystem"), FunctionContext), ELogVerbosity::Warning);
		}
	}

	void QueryIDFromPlayerController(APlayerController* PlayerController)
	{
		UserID.Reset();
		//return const_cast<FUniqueNetId*>(UniqueNetIdPtr);
		if (APlayerState* PlayerState = (PlayerController != NULL) ? PlayerController->PlayerState : NULL)
		{
			UserID = PlayerState->UniqueId.GetUniqueNetId();
			if (!UserID.IsValid())
			{
				FFrame::KismetExecutionMessage(*FString::Printf(TEXT("%s - Cannot map local player to unique net ID"), FunctionContext), ELogVerbosity::Warning);
			}
		}
		else
		{
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("%s - Invalid player state"), FunctionContext), ELogVerbosity::Warning);
		}
	}


	bool IsValid() const
	{
		return UserID.IsValid() && (OnlineSub != nullptr);
	}

public:
	//TSharedPtr<const FUniqueNetId>& GetUniqueNetId()
	TSharedPtr</*class*/ const FUniqueNetId> UserID;
	IOnlineSubsystem* const OnlineSub;
	const TCHAR* FunctionContext;
};
class FOnlineSearchSettingsEx : public FOnlineSearchSettings
{
	/**
	*	Sets a key value pair combination that defines a search parameter
	*
	* @param Key key for the setting
	* @param Value value of the setting
	* @param InType type of comparison
	*/
public:

	void HardSet(FName Key, const FVariantData& Value, EOnlineComparisonOpRedux::Type CompOp)
	{
		FOnlineSessionSearchParam* SearchParam = SearchParams.Find(Key);

		EOnlineComparisonOp::Type op;

		switch (CompOp)
		{
		case EOnlineComparisonOpRedux::Equals: op = EOnlineComparisonOp::Equals; break;
		case EOnlineComparisonOpRedux::GreaterThan: op = EOnlineComparisonOp::GreaterThan; break;
		case EOnlineComparisonOpRedux::GreaterThanEquals: op = EOnlineComparisonOp::GreaterThanEquals; break;
		case EOnlineComparisonOpRedux::LessThan: op = EOnlineComparisonOp::LessThan; break;
		case EOnlineComparisonOpRedux::LessThanEquals: op = EOnlineComparisonOp::LessThanEquals; break;
		case EOnlineComparisonOpRedux::NotEquals: op = EOnlineComparisonOp::NotEquals; break;
		default: op = EOnlineComparisonOp::Equals; break;
		}

		if (SearchParam)
		{
			SearchParam->Data = Value;
			SearchParam->ComparisonOp = op;
		}
		else
		{
			FOnlineSessionSearchParam searchSetting((int)0, op);
			searchSetting.Data = Value;
			SearchParams.Add(Key, searchSetting);
		}
	}
};

#define INVALID_INDEX -1