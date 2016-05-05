// Copyright 2012-2018 Helical Media Productions LLC. All Rights Reserved.

#include "projectvelorum.h"
#include "GetPlayerSteamID.h"

UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Player SteamID", CompactNodeTitle = "GetSteamID"), Category = "Online")
static bool GetControllerNetworkID(APlayerController* PlayerController, FString& ID, bool bAppendPort);