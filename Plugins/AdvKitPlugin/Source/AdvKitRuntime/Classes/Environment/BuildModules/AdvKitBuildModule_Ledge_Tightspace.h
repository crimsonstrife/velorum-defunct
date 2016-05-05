#pragma once

#include "Environment/AdvKitTransitionBuilderModule.h"
#include "AdvKitBuildModule_Ledge_Tightspace.generated.h"

class AAdvKitZone;
class AAdvKitCharacter;
/**
 * @brief Implementation of a transition builder module to create transitions between ledge and tight space zones
 */
UCLASS(Blueprintable, meta = (DisplayName = "Transitions between Ledge and Tight Space"))
class ADVKITRUNTIME_API UAdvKitBuildModule_Ledge_Tightspace : public UAdvKitTransitionBuilderModule
{
	GENERATED_BODY()

public:
	virtual bool CanCreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone) override;

protected:

	virtual bool GatherPotentialTargetZones_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone, TArray<AAdvKitZone*>& OutZones) override;
	virtual bool CreateTransitionBetween_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone) override;

	virtual bool CreateTransitionLedgeToTightSpace(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* Ledge, AAdvKitZone* TightSpace);
	virtual bool CreateTransitionTightSpaceToLedge(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* TightSpace, AAdvKitZone* Ledge);
};
