#pragma once

#include "Runtime/Core/Definitions.h"

class IModule
{
};

class RenderModule : public IModule
{
};

class TaskFlowModule : public IModule
{
};

class SpdLogModule : public IModule
{
};

struct WindowCreateInfo
{
	uint32_t Width = 0u;
	uint32_t Height = 0u;
	uint32_t MinWidth = 0;
	uint32_t MinHeight = 0u;
	std::string_view Title;
};

class Engine
{
public:
protected:
private:
};

