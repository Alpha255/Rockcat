#pragma once

#include "Runtime/Core/Definitions.h"

class EXPORT_API IModule
{
public:
	virtual ~IModule() = default;

	virtual void OnStartup() {}
	virtual void OnShutdown() {}
};
