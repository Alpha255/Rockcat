#include "Runtime/Core/DynamicLinkLibrary.h"
#include "Runtime/Core/PlatformMisc.h"
#include "Runtime/Engine/Services/SpdLogService.h"

#if PLATFORM_WIN32

#include <Windows.h>
#include <windowsx.h>
#include <shlobj.h>

DynamicLinkLibrary::DynamicLinkLibrary(const char8_t* ModuleName)
{
	m_Handle = reinterpret_cast<void*>(::LoadLibraryA(ModuleName));
	VERIFY_WITH_PLATFORM_MESSAGE(m_Handle);
}

void* DynamicLinkLibrary::GetProcAddress(const char8_t* FunctionName)
{
	assert(m_Handle);
	return ::GetProcAddress(reinterpret_cast<::HMODULE>(m_Handle), FunctionName);
}

DynamicLinkLibrary::~DynamicLinkLibrary()
{
	VERIFY_WITH_PLATFORM_MESSAGE(::FreeLibrary(reinterpret_cast<::HMODULE>(m_Handle)) != 0);
}

struct ComponentObjectModelLibrary
{
	ComponentObjectModelLibrary()
	{
		VERIFY_WITH_PLATFORM_MESSAGE(::CoInitializeEx(nullptr, COINIT_MULTITHREADED) == S_OK);
	}

	~ComponentObjectModelLibrary()
	{
		::CoUninitialize();
	}
};

#endif