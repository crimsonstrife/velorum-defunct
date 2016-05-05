// Copyright 2012-2018 Helical Media Productions LLC. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GetPlayerSteamID.generated.h"

/**
*
*/
UCLASS()
class PROJECTVELORUM_API UGetPlayerSteamID : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		bool UGetPlayerSteamID::GetControllerNetworkID(APlayerController* PlayerController, FString& ID, bool bAppendPort)
	{
		if (!PlayerController) return false;
		if (!PlayerController->PlayerState) return false;

		//ID = PlayerController->PlayerState->GetNetDriver()->LowLevelGetNetworkNumber();
		ID = PlayerController->PlayerState->UniqueId->ToString();

		if (!bAppendPort)
		{
			FString null;
			ID.Split(FString(":"), &ID, &null, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		}
		return true;
	}


};
