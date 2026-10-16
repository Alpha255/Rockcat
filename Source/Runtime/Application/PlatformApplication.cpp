#include "Application/PlatformApplication.h"

GenericPlatformApplication& GenericPlatformApplication::Get()
{
	static GenericPlatformApplication Instance;
	return Instance;
}

void GenericPlatformApplication::SetWindowTitle(const char* Title)
{

}

void GenericPlatformApplication::Loop()
{
	Initialize();

	while (!IsQuit())
	{
#if WITH_EDITOR
		//m_Editor->Tick(0.0f);
#else
		//m_GameInstance->Tick(0.0f);
#endif
	}

	Finalize();
}

void GenericPlatformApplication::CreateMainWindow(uint32_t Width, uint32_t Height)
{
}

void GenericPlatformApplication::Initialize()
{
	// Initialize TaskExecutorManager
	// Initialize Renderbackend
	// Initialize AssetRegistry
}

void GenericPlatformApplication::Finalize()
{
	// Finalize TaskExecutorManager
	// Finalize Renderbackend
	// Finalize AssetRegistry
}

void GenericPlatformApplication::Tick(float ElapsedSeconds)
{
}
