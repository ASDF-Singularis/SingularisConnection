#pragma once

#include "Modules/ModuleManager.h"

class FSingularisConnectionModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
