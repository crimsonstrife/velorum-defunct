#pragma once

#include "AdvKitTargetPoint.generated.h"

class AAdvKitCharacter;

/**
 * @brief This actor defines a point that can be targeted by characters or weapons.
 */
UCLASS()
class ADVKITRUNTIME_API AAdvKitTargetPoint : public ATargetPoint
{
	GENERATED_BODY()

public:

	/** Constructor */
	AAdvKitTargetPoint(const FObjectInitializer& ObjectInitializer);

	/** True if the target point is currently active */
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bCanBeTargeted;


	/** 
	 * Gets the actor that is targeted by this point.
	 * @return The actual target of this point
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	AActor* GetTargetedActor();

	/** 
	 * @return True if this point is currently able to be targeted
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	bool IsValidTarget();

};
