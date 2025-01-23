#pragma once

#include "Core/Definitions.h"

class IModule
{
public:
	virtual ~IModule() = default;

	virtual void OnStartup() {}
	virtual void OnShutdown() {}
};

template<class T>
class IService : public NoneCopyable
{
public:
	virtual void OnStartup() {}
	virtual void OnShutdown() {}

	static T& Get()
	{
		static T s_Service;
		return s_Service;
	}
};