#pragma once

#include "Profile/CpuTimer.h"

class ApplicationRunner
{
public:
	ApplicationRunner(class BaseApplication* App)
	{
		GApplication.reset(App);
	}

	const class BaseApplication& GetApplication() const { return *GApplication; }

	void Run();
private:
	void Initialize();
	void Finalize();

	std::unique_ptr<class BaseApplication> GApplication;
	std::unique_ptr<CpuTimer> GTimer;
};

#define RUN_APPLICATION(ApplicationType, ConfigurationPath) \
	ApplicationRunner GApplicationRunner(new ApplicationType(ConfigurationPath));

