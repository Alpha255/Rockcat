#pragma once

#include "Engine/Profile/CpuTimer.h"

class ApplicationRunner
{
public:
	ApplicationRunner(class BaseApplication* App)
	{
		GApplication.reset(App);
	}

	void Run();
private:
	void Initialize();
	void Finalize();

	std::unique_ptr<class BaseApplication> GApplication;
	std::unique_ptr<CpuTimer> GTimer;
};

#define RUN_APPLICATION(ApplicationType, ConfigurationPath) \
	ApplicationRunner GApplicationRunner(new ApplicationType(ConfigurationPath));

