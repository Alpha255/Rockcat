#pragma once

#include "Runtime/Core/Definitions.h"

class IModule
{
public:
	virtual ~IModule() = default;

	virtual void OnStartup() {}
	virtual void OnShutdown() {}
};
