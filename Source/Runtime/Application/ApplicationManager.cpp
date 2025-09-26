#include "Application/ApplicationManager.h"
#include "Application/BaseApplication.h"

void ApplicationManager::RegisterApplication(BaseApplication* Application)
{
	assert(Application);
	m_Applications.push_back(Application);
}

void ApplicationManager::DestoryApplications()
{
	for (auto Application : m_Applications)
	{
		delete Application;
	}
	m_Applications.clear();
}

#if PLATFORM_WIN32
#include <Windows.h>

int32_t WINAPI WinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*Commandline*/, _In_ int32_t /*ShowCmd*/)
{
	ApplicationManager::Get().m_Applications[0]->Run();

	ApplicationManager::Get().DestoryApplications();

	return 0;
}
#else
	#error Unknown platform
#endif
