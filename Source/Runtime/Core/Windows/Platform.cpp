#include "Runtime/Core/Platform.h"
#include "Runtime/Core/File.h"
#include "Runtime/Core/Logger.h"

#if defined(PLATFORM_WIN32)
	#include <Windows.h>
	#include <windowsx.h>
	#include <shlobj.h>
#else
	#error Unknown platform!
#endif

namespace Platform
{
#if defined(PLATFORM_WIN32)
	std::string GetErrorMessage(uint32_t ErrCode)
	{
		static std::shared_ptr<char8_t> Buffer(new char8_t[UINT16_MAX]());
		memset(Buffer.get(), 0, UINT16_MAX);

		VERIFY(::FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			ErrCode == ~0u ? ::GetLastError() : ErrCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			Buffer.get(),
			UINT16_MAX,
			nullptr) != 0);

		return std::string(Buffer.get());
	}

	std::string GetCurrentModuleDirectory()
	{
		static std::shared_ptr<char8_t> Buffer(new char8_t[UINT16_MAX]());
		memset(Buffer.get(), 0, UINT16_MAX);

		VERIFY_PLATFORM(::GetModuleFileNameA(nullptr, Buffer.get(), UINT16_MAX) != 0);
		return File::Directory(std::string(Buffer.get()));
	}

	std::string GetCurrentWorkingDirectory()
	{
		static std::shared_ptr<char8_t> Buffer(new char8_t[UINT16_MAX]());
		memset(Buffer.get(), 0, UINT16_MAX);

		VERIFY_PLATFORM(::GetCurrentDirectoryA(UINT16_MAX, Buffer.get()) != 0);
		return std::string(Buffer.get());
	}

	void SetCurrentWorkingDirectory(const char8_t* Path)
	{
		assert(File::Exists(Path));
		VERIFY_PLATFORM(::SetCurrentDirectoryA(Path) != 0);
	}

	void Sleep(uint32_t Milliseconds)
	{
		::Sleep(static_cast<::DWORD>(Milliseconds));
	}

	void ExecuteProcess(const char8_t* Commandline, bool8_t WaitDone)
	{
		::SECURITY_ATTRIBUTES Security
		{
			sizeof(::SECURITY_ATTRIBUTES),
			nullptr,
			true
		};

		::HANDLE Read = nullptr, Write = nullptr;
		VERIFY_PLATFORM(::CreatePipe(&Read, &Write, &Security, INT16_MAX) != 0);
		VERIFY_PLATFORM(::SetStdHandle(STD_OUTPUT_HANDLE, Write) != 0);

		/// If an error occurs, the ANSI version of this function (GetStartupInfoA) can raise an exception. 
		/// The Unicode version (GetStartupInfoW) does not fail
		::STARTUPINFOA StartupInfo;
		::GetStartupInfoA(&StartupInfo);
		StartupInfo.cb = sizeof(::STARTUPINFOA);
		StartupInfo.dwFlags = STARTF_USESTDHANDLES;
		StartupInfo.hStdInput = Read;
		StartupInfo.hStdOutput = Write;

		static std::shared_ptr<char8_t> Buffer(new char8_t[INT16_MAX]());
		memset(Buffer.get(), 0, INT16_MAX);

		::PROCESS_INFORMATION ProcessInfo;
		if (::CreateProcessA(
			nullptr,
			const_cast<::LPSTR>(Commandline),
			nullptr,
			nullptr,
			true,
			CREATE_NO_WINDOW,
			nullptr,
			nullptr,
			&StartupInfo,
			&ProcessInfo))
		{
			if (WaitDone)
			{
				::DWORD Exit = 0u;
				::WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
				if (::GetExitCodeProcess(ProcessInfo.hProcess, &Exit) && Exit != 0u)
				{
					::DWORD Bytes = 0u;
					VERIFY_PLATFORM(::ReadFile(Read, Buffer.get(), INT16_MAX, &Bytes, nullptr) != 0);
					//buffer[bytes] = '\0';
					LOG_ERROR("Failed to execute command \"%s\"", Buffer.get());
				}
				else
				{
					VERIFY_PLATFORM(0);
				}

				/// STILL_ACTIVE
			}

			::CloseHandle(ProcessInfo.hThread);
			::CloseHandle(ProcessInfo.hProcess);

			::CloseHandle(Read);
			::CloseHandle(Write);

			return;
		}

		VERIFY_PLATFORM(0);
	}

	std::string GetEnvironmentVariables(const char8_t* Var)
	{
		static std::shared_ptr<char8_t> Buffer(new char8_t[UINT16_MAX]());
		memset(Buffer.get(), 0, UINT16_MAX);

		VERIFY_PLATFORM(::GetEnvironmentVariableA(Var, Buffer.get(), UINT16_MAX) != 0);
		return std::string(Buffer.get());
	}

	uint64_t GetCurrentModuleHandle()
	{
		::HMODULE Handle = ::GetModuleHandleA(nullptr);
		VERIFY_PLATFORM(Handle);
		return reinterpret_cast<uint64_t>(Handle);
	}

	Guid CreateGUID()
	{
		Guid Ret;
		VERIFY_PLATFORM(::CoCreateGuid(reinterpret_cast<::GUID*>(&Ret)) == S_OK);
		return Ret;
	}

	Math::Vector2 GetCurrentCursorPosition()
	{
		::POINT Pos;
		::GetCursorPos(&Pos);

		return Math::Vector2(static_cast<float32_t>(Pos.x), static_cast<float32_t>(Pos.y));
	}

	DynamicLibrary::DynamicLibrary(const char8_t* Name)
	{
		m_Handle = reinterpret_cast<uint64_t>(::LoadLibraryA(Name));
		VERIFY_PLATFORM(m_Handle);
	}

	void* DynamicLibrary::GetProcAddress(const char8_t* FuncName)
	{
		assert(m_Handle);
		return ::GetProcAddress(reinterpret_cast<::HMODULE>(m_Handle), FuncName);
	}

	DynamicLibrary::~DynamicLibrary()
	{
		VERIFY_PLATFORM(::FreeLibrary(reinterpret_cast<::HMODULE>(m_Handle)) != 0);
	}
#else
	#error Unknown platform!
#endif
}

Guid Guid::Create()
{
	return Platform::CreateGUID();
}

