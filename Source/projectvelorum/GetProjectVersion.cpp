// Copyright 2012-2018 Helical Media Productions LLC. All Rights Reserved.

#include "projectvelorum.h"
#include "GetProjectVersion.h"


FString UGetProjectVersion::GetProjectVersion() {
	FString ProjectVersion;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectVersion"),
		ProjectVersion,
		GGameIni
	);
	return ProjectVersion;
}