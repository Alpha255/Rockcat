#include "System.h"
#include "Services/SpdLogService.h"

#if PLATFORM_WIN32

#include <Windows.h>
#include <windowsx.h>
#include <shlobj.h>

#define FILE_PATH_LENGTH_MAX UINT16_MAX

std::string System::GetErrorMessage(uint32_t ErrorCode)
{
	static wchar_t s_Buffer[FILE_PATH_LENGTH_MAX];
	memset(s_Buffer, 0, sizeof(s_Buffer));

	VERIFY(::FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		ErrorCode == ~0u ? ::GetLastError() : ErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		s_Buffer,
		FILE_PATH_LENGTH_MAX,
		nullptr) != 0);

	return StringUtils::ToMultiByte(s_Buffer);
}

std::filesystem::path System::GetApplicationDirectory()
{
	static wchar_t s_Buffer[FILE_PATH_LENGTH_MAX];
	memset(s_Buffer, 0, sizeof(s_Buffer));

	VERIFY_WITH_SYSTEM_MESSAGE(::GetModuleFileNameW(nullptr, s_Buffer, sizeof(s_Buffer)) != 0);
	return std::filesystem::path(s_Buffer).parent_path();
}

std::filesystem::path System::GetWorkingDirectory()
{
	static wchar_t s_Buffer[FILE_PATH_LENGTH_MAX];
	memset(s_Buffer, 0, sizeof(s_Buffer));

	VERIFY_WITH_SYSTEM_MESSAGE(::GetCurrentDirectoryW(sizeof(s_Buffer), s_Buffer) != 0);
	return std::filesystem::path(s_Buffer);
}

void System::SetWorkingDirectory(const std::filesystem::path& Directory)
{
	assert(std::filesystem::exists(Directory));
	VERIFY_WITH_SYSTEM_MESSAGE(::SetCurrentDirectoryW(Directory.c_str()) != 0);
}

void System::Sleep(uint32_t Milliseconds)
{
	::Sleep(static_cast<::DWORD>(Milliseconds));
}

void System::ExecuteProcess(const char* Commandline, bool WaitDone)
{
	::SECURITY_ATTRIBUTES Security
	{
		sizeof(::SECURITY_ATTRIBUTES),
			nullptr,
			true
	};

	::HANDLE Read = nullptr, Write = nullptr;
	VERIFY_WITH_SYSTEM_MESSAGE(::CreatePipe(&Read, &Write, &Security, INT16_MAX) != 0);
	VERIFY_WITH_SYSTEM_MESSAGE(::SetStdHandle(STD_OUTPUT_HANDLE, Write) != 0);

	/// If an error occurs, the ANSI version of this function (GetStartupInfoA) can raise an exception. 
	/// The Unicode version (GetStartupInfoW) does not fail
	::STARTUPINFOW StartupInfo;
	::GetStartupInfoW(&StartupInfo);
	StartupInfo.cb = sizeof(::STARTUPINFOA);
	StartupInfo.dwFlags = STARTF_USESTDHANDLES;
	StartupInfo.hStdInput = Read;
	StartupInfo.hStdOutput = Write;

	static wchar_t s_Buffer[FILE_PATH_LENGTH_MAX];
	memset(s_Buffer, 0, sizeof(s_Buffer));

	std::wstring wComandline = StringUtils::ToWide(Commandline);

	::PROCESS_INFORMATION ProcessInfo;
	if (::CreateProcessW(
		nullptr,
		const_cast<LPWSTR>(wComandline.c_str()),
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
				VERIFY_WITH_SYSTEM_MESSAGE(::ReadFile(Read, s_Buffer, sizeof(s_Buffer), &Bytes, nullptr) != 0);
				//buffer[bytes] = '\0';
				std::string ErrorMessage = StringUtils::ToMultiByte(s_Buffer);
				LOG_ERROR("Failed to executing process \"%s\"", ErrorMessage);
			}
			else
			{
				VERIFY_WITH_SYSTEM_MESSAGE(0);
			}

			/// STILL_ACTIVE
		}

		::CloseHandle(ProcessInfo.hThread);
		::CloseHandle(ProcessInfo.hProcess);

		::CloseHandle(Read);
		::CloseHandle(Write);

		return;
	}

	VERIFY_WITH_SYSTEM_MESSAGE(0);
}

std::string System::GetEnvironmentVariables(const char* Variable)
{
	static wchar_t s_Buffer[FILE_PATH_LENGTH_MAX];
	memset(s_Buffer, 0, sizeof(s_Buffer));

	std::wstring wVariable = StringUtils::ToWide(Variable);

	VERIFY_WITH_SYSTEM_MESSAGE(::GetEnvironmentVariableW(wVariable.c_str(), s_Buffer, sizeof(s_Buffer)) != 0);
	return StringUtils::ToMultiByte(s_Buffer);
}

void* System::GetApplicationInstance()
{
	::HMODULE Handle = ::GetModuleHandleW(nullptr);
	VERIFY_WITH_SYSTEM_MESSAGE(Handle);
	return reinterpret_cast<void*>(Handle);
}

Guid System::CreateGUID()
{
	Guid Ret;
	VERIFY_WITH_SYSTEM_MESSAGE(::CoCreateGuid(reinterpret_cast<::GUID*>(&Ret)) == S_OK);
	return Ret;
}

Math::Vector2 System::GetCursorPosition()
{
	::POINT Pos;
	::GetCursorPos(&Pos);

	return Math::Vector2(static_cast<float>(Pos.x), static_cast<float>(Pos.y));
}

size_t System::GetHardwareConcurrencyThreadsCount(bool UseHyperThreading)
{
	std::unique_ptr<uint8_t> Buffer;
	::DWORD BufferSize = 0;
	size_t PhysicalCoreCount = 0u;
	size_t LogicalCoreCount = 0u;

	if(!::GetLogicalProcessorInformationEx(::LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll, (::PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)Buffer.get(), &BufferSize) &&
		::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		Buffer.reset(new uint8_t[BufferSize]());
		if (::GetLogicalProcessorInformationEx(::LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll, (::PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)Buffer.get(), &BufferSize))
		{
			uint8_t* BufferPtr = Buffer.get();
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
	return System::CreateGUID();
}

void System::SetThreadPriority(std::thread::id ThreadID, Task::EPriority Priority)
{
	static const char* s_ThreadPriorityNames[] =
	{
		"Low",
		"Normal",
		"High",
		"Critical",		
	};

	std::stringstream Stream;
	Stream << ThreadID;

	::DWORD DwordThreadID = std::stoul(Stream.str());
	::HANDLE ThreadHandle = ::OpenThread(THREAD_ALL_ACCESS, false, DwordThreadID);
	VERIFY_WITH_SYSTEM_MESSAGE(ThreadHandle);

	int32_t ThreadPriority = THREAD_PRIORITY_NORMAL;
	switch (Priority)
	{
	case Task::EPriority::Critical:
		ThreadPriority = THREAD_PRIORITY_HIGHEST;
		break;
	case Task::EPriority::High:
		ThreadPriority = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case Task::EPriority::Low:
		ThreadPriority = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	}
	VERIFY_WITH_SYSTEM_MESSAGE(::SetThreadPriority(ThreadHandle, ThreadPriority) != 0);

	LOG_INFO("Set priority of thread {} to {}", DwordThreadID, s_ThreadPriorityNames[(size_t)Priority]);
}

void System::PumpMessages()
{
	::MSG Message;
	if (::PeekMessageW(&Message, nullptr, 0u, 0u, PM_REMOVE))
	{
		::TranslateMessage(&Message);
		::DispatchMessageW(&Message);
	}
}

#endif // PLATFORM_WIN32

