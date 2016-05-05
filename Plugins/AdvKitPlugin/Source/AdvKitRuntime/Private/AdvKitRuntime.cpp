#include "AdvKitRuntime.h"

DEFINE_LOG_CATEGORY(LogAdvKit);

/** @brief Module to register runtime classes of the adventure kit. */
class FAdvKitRuntime : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FAdvKitRuntime, AdvKitRuntime)


void FAdvKitRuntime::StartupModule()
{

}


void FAdvKitRuntime::ShutdownModule()
{

}



