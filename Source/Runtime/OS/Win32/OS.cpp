#include "OS/OS.h"
#include "Services/SpdLogService.h"

#if PLATFORM_WIN32

#include <Windows.h>
#include <windowsx.h>
#include <shlobj.h>

std::string OS::GetErrorMessage(uint32_t ErrorCode)
{
	static wchar_t s_Buffer[UINT16_MAX];
	memset(s_Buffer, 0, sizeof(s_Buffer));

	VERIFY(::FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		ErrorCode == ~0u ? ::GetLastError() : ErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		s_Buffer,
		UINT16_MAX,
		nullptr) != 0);

	return String::ToMultiByte(s_Buffer);
}

std::filesystem::path OS::GetApplicationDirectory()
{
	static wchar_t s_Buffer[UINT16_MAX];
	memset(s_Buffer, 0, sizeof(s_Buffer));

	VERIFY_WITH_OS_MESSAGE(::GetModuleFileNameW(nullptr, s_Buffer, sizeof(s_Buffer)) != 0);
	return std::filesystem::path(s_Buffer).parent_path();
}

std::filesystem::path OS::GetWorkingDirectory()
{
	static wchar_t s_Buffer[UINT16_MAX];
	memset(s_Buffer, 0, sizeof(s_Buffer));

	VERIFY_WITH_OS_MESSAGE(::GetCurrentDirectoryW(sizeof(s_Buffer), s_Buffer) != 0);
	return std::filesystem::path(s_Buffer);
}

void OS::SetWorkingDirectory(const std::filesystem::path& Directory)
{
	assert(std::filesystem::exists(Directory));
	VERIFY_WITH_OS_MESSAGE(::SetCurrentDirectoryW(Directory.c_str()) != 0);
}

void OS::Sleep(uint32_t Milliseconds)
{
	::Sleep(static_cast<::DWORD>(Milliseconds));
}

void OS::ExecuteProcess(const char* Commandline, bool WaitDone)
{
	::SECURITY_ATTRIBUTES Security
	{
		sizeof(::SECURITY_ATTRIBUTES),
			nullptr,
			true
	};

	::HANDLE Read = nullptr, Write = nullptr;
	VERIFY_WITH_OS_MESSAGE(::CreatePipe(&Read, &Write, &Security, INT16_MAX) != 0);
	VERIFY_WITH_OS_MESSAGE(::SetStdHandle(STD_OUTPUT_HANDLE, Write) != 0);

	/// If an error occurs, the ANSI version of this function (GetStartupInfoA) can raise an exception. 
	/// The Unicode version (GetStartupInfoW) does not fail
	::STARTUPINFOW StartupInfo;
	::GetStartupInfoW(&StartupInfo);
	StartupInfo.cb = sizeof(::STARTUPINFOA);
	StartupInfo.dwFlags = STARTF_USESTDHANDLES;
	StartupInfo.hStdInput = Read;
	StartupInfo.hStdOutput = Write;

	static wchar_t s_Buffer[UINT16_MAX];
	memset(s_Buffer, 0, sizeof(s_Buffer));

	std::wstring wComandline = String::ToWide(Commandline);

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
				VERIFY_WITH_OS_MESSAGE(::ReadFile(Read, s_Buffer, sizeof(s_Buffer), &Bytes, nullptr) != 0);
				//buffer[bytes] = '\0';
				std::string ErrorMessage = String::ToMultiByte(s_Buffer);
				LOG_ERROR(LogDefault, "Failed to executing process \"%s\"", ErrorMessage);
			}
			else
			{
				VERIFY_WITH_OS_MESSAGE(0);
			}

			/// STILL_ACTIVE
		}

		::CloseHandle(ProcessInfo.hThread);
		::CloseHandle(ProcessInfo.hProcess);

		::CloseHandle(Read);
		::CloseHandle(Write);

		return;
	}

	VERIFY_WITH_OS_MESSAGE(0);
}

std::string OS::GetEnvironmentVariables(const char* Variable)
{
	static wchar_t s_Buffer[UINT16_MAX];
	memset(s_Buffer, 0, sizeof(s_Buffer));

	std::wstring wVariable = String::ToWide(Variable);

	VERIFY_WITH_OS_MESSAGE(::GetEnvironmentVariableW(wVariable.c_str(), s_Buffer, sizeof(s_Buffer)) != 0);
	return String::ToMultiByte(s_Buffer);
}

void* OS::GetApplicationInstance()
{
	::HMODULE Handle = ::GetModuleHandleW(nullptr);
	VERIFY_WITH_OS_MESSAGE(Handle);
	return reinterpret_cast<void*>(Handle);
}

Guid OS::CreateGUID()
{
	Guid Ret;
	VERIFY_WITH_OS_MESSAGE(::CoCreateGuid(reinterpret_cast<::GUID*>(&Ret)) == S_OK);
	return Ret;
}

Math::Vector2 OS::GetCursorPosition()
{
	::POINT Pos;
	::GetCursorPos(&Pos);

	return Math::Vector2(static_cast<float>(Pos.x), static_cast<float>(Pos.y));
}

size_t OS::GetHardwareConcurrencyThreadsCount(bool UseHyperThreading)
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
	return OS::CreateGUID();
}

void OS::SetThreadPriority(std::thread::id ThreadID, TFTask::EPriority Priority)
{
	std::stringstream Stream;
	Stream << ThreadID;

	::DWORD DwordThreadID = std::stoul(Stream.str());
	::HANDLE ThreadHandle = ::OpenThread(THREAD_ALL_ACCESS, false, DwordThreadID);
	VERIFY_WITH_OS_MESSAGE(ThreadHandle);

	int32_t ThreadPriority = THREAD_PRIORITY_NORMAL;
	switch (Priority)
	{
	case TFTask::EPriority::Critical:
		ThreadPriority = THREAD_PRIORITY_HIGHEST;
		break;
	case TFTask::EPriority::High:
		ThreadPriority = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case TFTask::EPriority::Low:
		ThreadPriority = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	}
	VERIFY_WITH_OS_MESSAGE(::SetThreadPriority(ThreadHandle, ThreadPriority) != 0);
}

#endif // PLATFORM_WIN32

