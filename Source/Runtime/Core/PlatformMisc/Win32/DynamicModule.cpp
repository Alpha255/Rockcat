#include "Runtime/Core/DynamicModule.h"
#include "Runtime/Engine/Engine.h"
#include "Runtime/Core/PlatformMisc.h"

#if PLATFORM_WIN32

#include <Windows.h>
#include <windowsx.h>
#include <shlobj.h>

DynamicModule::DynamicModule(const char8_t* ModuleName)
{
	m_Handle = reinterpret_cast<void*>(::LoadLibraryA(ModuleName));
	VERIFY_WITH_PLATFORM_MESSAGE(m_Handle);
}

void* DynamicModule::GetProcAddress(const char8_t* FunctionName)
{
	assert(m_Handle);
	return ::GetProcAddress(reinterpret_cast<::HMODULE>(m_Handle), FunctionName);
}

DynamicModule::~DynamicModule()
{
	VERIFY_WITH_PLATFORM_MESSAGE(::FreeLibrary(reinterpret_cast<::HMODULE>(m_Handle)) != 0);
}

#endif