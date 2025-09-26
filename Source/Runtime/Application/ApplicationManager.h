#pragma once

#include "Core/Definitions.h"
#include "Core/Singleton.h"

class ApplicationManager
{
public:
	void RegisterApplication(class BaseApplication* Application);
protected:
};

#define RUN_APPLICATION(Application, SettingsFile)

#define REGISTER_APPLICATION(ApplicationClass, SettingsFile)

#define REGISTER_AND_RUN_APPLICATION(ApplicationClass, SettingsFile)