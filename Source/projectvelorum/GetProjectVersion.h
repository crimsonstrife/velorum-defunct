// Copyright 2012-2018 Helical Media Productions LLC. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GetProjectVersion.generated.h"

/**
*
*/
UCLASS()
class PROJECTVELORUM_API UGetProjectVersion : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Project Version", CompactNodeTitle = "ProjectVersion"), Category = "System Information")
		static FString GetProjectVersion();


};