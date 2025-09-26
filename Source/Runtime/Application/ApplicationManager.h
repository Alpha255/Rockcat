#pragma once

#include "Core/Definitions.h"
#include "Core/Singleton.h"

class ApplicationManager : public Singleton<ApplicationManager>
{
public:
	void RegisterApplication(class BaseApplication* Application);
	void DestoryApplications();

	std::vector<class BaseApplication*> m_Applications;
};

template<class TApplication>
struct ApplicationRegister
{
	ApplicationRegister(const char* SettingsFile)
	{
		ApplicationManager::Get().RegisterApplication(new TApplication(SettingsFile));
	}
};

#define REGISTER_APPLICATION(ApplicationClass) \
	static ApplicationRegister<ApplicationClass> CAT(GRegisterApp_##ApplicationClass, __LINE__)(#ApplicationClass "Settings.json");