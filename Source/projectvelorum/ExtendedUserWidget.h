// Copyright 2012-2018 Helical Media Productions LLC. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "ExtendedUserWidget.generated.h"

/**
*
*/
UCLASS()
class PROJECTVELORUM_API UExtendedUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extended User Widget")
		FString ExtendedWidget;


};