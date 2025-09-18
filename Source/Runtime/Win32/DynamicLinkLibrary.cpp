#include "DynamicLinkLibrary.h"
#include "System.h"
#include "Services/SpdLogService.h"

#if PLATFORM_WIN32

#include <Windows.h>
#include <windowsx.h>
#include <shlobj.h>

DynamicLinkLibrary::DynamicLinkLibrary(const char* ModuleName)
{
	m_Handle = reinterpret_cast<void*>(::LoadLibraryA(ModuleName));
	VERIFY_WITH_SYSTEM_MESSAGE(m_Handle);
}

void* DynamicLinkLibrary::GetProcAddress(const char* FunctionName)
{
	assert(m_Handle);
	return ::GetProcAddress(reinterpret_cast<::HMODULE>(m_Handle), FunctionName);
}

DynamicLinkLibrary::~DynamicLinkLibrary()
{
	VERIFY_WITH_SYSTEM_MESSAGE(::FreeLibrary(reinterpret_cast<::HMODULE>(m_Handle)) != 0);
}

struct ComponentObjectModelLibrary
{
	ComponentObjectModelLibrary()
	{
		VERIFY_WITH_SYSTEM_MESSAGE(::CoInitializeEx(nullptr, COINIT_MULTITHREADED) == S_OK);
	}

	~ComponentObjectModelLibrary()
	{
		::CoUninitialize();
	}
};

#endif // PLATFORM_WIN32