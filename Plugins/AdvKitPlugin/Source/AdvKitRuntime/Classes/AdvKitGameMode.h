// Copyright 2015 Pascal Krabbe

#pragma once

#include "AdvKitGameMode.generated.h"

/**
 * @brief Fairly rudimentary game mode extension, simply to set the AdvKit classes as defaults
 *
 * Note: May be removed in the future
 */
UCLASS()
class ADVKITRUNTIME_API AAdvKitGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	/**
	 * Constructor
	 */
	AAdvKitGameMode(const FObjectInitializer& ObjectInitializer);
};



