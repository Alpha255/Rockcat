#include "Runtime/Core/PlatformMisc.h"
#include "Runtime/Engine/Services/SpdLogService.h"

#if defined(PLATFORM_WIN32)

#include <Windows.h>
#include <windowsx.h>
#include <shlobj.h>

#define FILE_PATH_LENGTH_MAX UINT16_MAX

std::string PlatformMisc::GetErrorMessage(uint32_t ErrorCode)
{
	static char s_Buffer[FILE_PATH_LENGTH_MAX];
	memset(s_Buffer, 0, FILE_PATH_LENGTH_MAX);

	VERIFY(::FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		ErrorCode == ~0u ? ::GetLastError() : ErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		s_Buffer,
		FILE_PATH_LENGTH_MAX,
		nullptr) != 0);

	return std::string(s_Buffer);
}

std::filesystem::path PlatformMisc::GetCurrentModuleDirectory()
{
	static wchar_t s_Buffer[FILE_PATH_LENGTH_MAX];
	memset(s_Buffer, 0, FILE_PATH_LENGTH_MAX);

	VERIFY_WITH_PLATFORM_MESSAGE(::GetModuleFileName(nullptr, s_Buffer, FILE_PATH_LENGTH_MAX) != 0);
	return std::filesystem::path(s_Buffer).parent_path();
}

std::filesystem::path PlatformMisc::GetCurrentWorkingDirectory()
{
	static wchar_t s_Buffer[FILE_PATH_LENGTH_MAX];
	memset(s_Buffer, 0, FILE_PATH_LENGTH_MAX);

	VERIFY_WITH_PLATFORM_MESSAGE(::GetCurrentDirectory(FILE_PATH_LENGTH_MAX, s_Buffer) != 0);
	return std::filesystem::path(s_Buffer);
}

void PlatformMisc::SetCurrentWorkingDirectory(const std::filesystem::path& Directory)
{
	assert(std::filesystem::exists(Directory));
	VERIFY_WITH_PLATFORM_MESSAGE(::SetCurrentDirectory(Directory.c_str()) != 0);
}

void PlatformMisc::Sleep(uint32_t Milliseconds)
{
	::Sleep(static_cast<::DWORD>(Milliseconds));
}

void PlatformMisc::ExecuteProcess(const char* Commandline, bool8_t WaitDone)
{
	::SECURITY_ATTRIBUTES Security
	{
		sizeof(::SECURITY_ATTRIBUTES),
			nullptr,
			true
	};

	::HANDLE Read = nullptr, Write = nullptr;
	VERIFY_WITH_PLATFORM_MESSAGE(::CreatePipe(&Read, &Write, &Security, INT16_MAX) != 0);
	VERIFY_WITH_PLATFORM_MESSAGE(::SetStdHandle(STD_OUTPUT_HANDLE, Write) != 0);

	/// If an error occurs, the ANSI version of this function (GetStartupInfoA) can raise an exception. 
	/// The Unicode version (GetStartupInfoW) does not fail
	::STARTUPINFOA StartupInfo;
	::GetStartupInfoA(&StartupInfo);
	StartupInfo.cb = sizeof(::STARTUPINFOA);
	StartupInfo.dwFlags = STARTF_USESTDHANDLES;
	StartupInfo.hStdInput = Read;
	StartupInfo.hStdOutput = Write;

	static char s_Buffer[FILE_PATH_LENGTH_MAX];
	memset(s_Buffer, 0, FILE_PATH_LENGTH_MAX);

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
				VERIFY_WITH_PLATFORM_MESSAGE(::ReadFile(Read, s_Buffer, FILE_PATH_LENGTH_MAX, &Bytes, nullptr) != 0);
				//buffer[bytes] = '\0';
				LOG_ERROR("Failed to executing process \"%s\"", s_Buffer);
			}
			else
			{
				VERIFY_WITH_PLATFORM_MESSAGE(0);
			}

			/// STILL_ACTIVE
		}

		::CloseHandle(ProcessInfo.hThread);
		::CloseHandle(ProcessInfo.hProcess);

		::CloseHandle(Read);
		::CloseHandle(Write);

		return;
	}

	VERIFY_WITH_PLATFORM_MESSAGE(0);
}

std::string PlatformMisc::GetEnvironmentVariables(const char* Variable)
{
	static char s_Buffer[FILE_PATH_LENGTH_MAX];
	memset(s_Buffer, 0, FILE_PATH_LENGTH_MAX);

	VERIFY_WITH_PLATFORM_MESSAGE(::GetEnvironmentVariableA(Variable, s_Buffer, FILE_PATH_LENGTH_MAX) != 0);
	return std::string(s_Buffer);
}

void* PlatformMisc::GetCurrentModuleHandle()
{
	::HMODULE Handle = ::GetModuleHandleA(nullptr);
	VERIFY_WITH_PLATFORM_MESSAGE(Handle);
	return reinterpret_cast<void*>(Handle);
}

Guid PlatformMisc::CreateGUID()
{
	Guid Ret;
	VERIFY_WITH_PLATFORM_MESSAGE(::CoCreateGuid(reinterpret_cast<::GUID*>(&Ret)) == S_OK);
	return Ret;
}

Math::Vector2 PlatformMisc::GetCurrentCursorPosition()
{
	::POINT Pos;
	::GetCursorPos(&Pos);

	return Math::Vector2(static_cast<float32_t>(Pos.x), static_cast<float32_t>(Pos.y));
}

size_t PlatformMisc::GetHardwareConcurrencyThreadsCount(bool8_t UseHyperThreading)
{
	std::unique_ptr<byte8_t> Buffer;
	::DWORD BufferSize = 0;
	size_t PhysicalCoreCount = 0u;
	size_t LogicalCoreCount = 0u;

	if(!::GetLogicalProcessorInformationEx(::LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll, (::PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)Buffer.get(), &BufferSize) &&
		::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		Buffer.reset(new byte8_t[BufferSize]());
		if (::GetLogicalProcessorInformationEx(::LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll, (::PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)Buffer.get(), &BufferSize))
		{
			byte8_t* BufferPtr = Buffer.get();
			while (BufferPtr < Buffer.get() + BufferSize)
			{
				::PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX ProcessorInfo = (::PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)BufferPtr;
				if (!ProcessorInfo)
				{
					break;
				}

				if (ProcessorInfo->Relationship == ::LOGICAL_PROCESSOR_RELATIONSHIP::RelationProcessorCore)
				{
					++PhysicalCoreCount;

					for (uint32_t Index = 0u; Index < ProcessorInfo->Processor.GroupCount; ++Index)
					{
						LogicalCoreCount += std::bitset<sizeof(::KAFFINITY) * CHAR_BIT>(ProcessorInfo->Processor.GroupMask[Index].Mask).count();
					}
				}
				BufferPtr += ProcessorInfo->Size;
			} 
		}
	}

	return UseHyperThreading ? LogicalCoreCount : PhysicalCoreCount;
}

Guid Guid::Create()
{
	return PlatformMisc::CreateGUID();
}

#endif

