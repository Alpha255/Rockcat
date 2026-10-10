#include "Misc/PlatformMisc.h"
#include "Core/SpdLogging.h"

#if PLATFORM_WIN32

#if defined(GetEnvironmentVariable)
#undef GetEnvironmentVariable

#include <Windows.h>

namespace PlatformMisc
{
	string GetErrorMessage(uint32_t ErrorCode)
	{
		std::vector<wchar_t> Buffer(UINT16_MAX, L'\0');

		if (!::FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			ErrorCode == ~0u ? ::GetLastError() : ErrorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			Buffer.data(),
			static_cast<::DWORD>(Buffer.size()),
			nullptr))
		{
			return string();
		}

		string Result = string::from_wide(Buffer.data());

		while (!Result.empty() && (Result.back() == '\r' || Result.back() == '\n' || Result.back() == ' '))
		{
			Result.pop_back();
		}

		return Result;
	}

	std::filesystem::path GetApplicationPath()
	{
		std::vector<wchar_t> Buffer(UINT16_MAX, L'\0');

		const ::DWORD Length = ::GetModuleFileNameW(nullptr, Buffer.data(), static_cast<::DWORD>(Buffer.size()));
		if (Length == 0u || Length >= Buffer.size())
		{
			VERIFY_WITH_SYSTEM_MESSAGE(0);
			return std::filesystem::path();
		}

		return std::filesystem::path(Buffer.data());
	}

	std::filesystem::path GetWorkingDirectory()
	{
		std::vector<wchar_t> Buffer(UINT16_MAX, L'\0');

		const ::DWORD Length = ::GetCurrentDirectoryW(static_cast<::DWORD>(Buffer.size()), Buffer.data());
		if (Length == 0u || Length >= Buffer.size())
		{
			VERIFY_WITH_SYSTEM_MESSAGE(0);
			return std::filesystem::path();
		}

		return std::filesystem::path(Buffer.data());
	}

	void SetWorkingDirectory(const std::filesystem::path& Directory)
	{
		assert(std::filesystem::exists(Directory));
		VERIFY_WITH_SYSTEM_MESSAGE(::SetCurrentDirectoryW(Directory.c_str()) != 0);
	}

	void Sleep(uint32_t Seconds)
	{
		::Sleep(static_cast<::DWORD>(static_cast<uint64_t>(Seconds) * 1000u));
	}

	void ExecuteProcess(const char* Commandline, bool WaitDone)
	{
		std::wstring WideCommandline = Utf8ToWide(Commandline);
		if (WideCommandline.empty())
		{
			return;
		}

		::SECURITY_ATTRIBUTES Security
		{
			sizeof(::SECURITY_ATTRIBUTES),
			nullptr,
			true
		};

		::HANDLE Read = nullptr, Write = nullptr;
		if (!::CreatePipe(&Read, &Write, &Security, 0))
		{
			VERIFY_WITH_SYSTEM_MESSAGE(0);
			return;
		}

		::STARTUPINFOW StartupInfo{};
		StartupInfo.cb = sizeof(::STARTUPINFOW);
		StartupInfo.dwFlags = STARTF_USESTDHANDLES;
		StartupInfo.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
		StartupInfo.hStdOutput = Write;
		StartupInfo.hStdError = Write;

		::PROCESS_INFORMATION ProcessInfo{};

		if (!::CreateProcessW(
			nullptr,
			WideCommandline.data(),
			nullptr,
			nullptr,
			true,
			CREATE_NO_WINDOW,
			nullptr,
			nullptr,
			&StartupInfo,
			&ProcessInfo))
		{
			::CloseHandle(Read);
			::CloseHandle(Write);
			VERIFY_WITH_SYSTEM_MESSAGE(0);
			return;
		}

		::CloseHandle(ProcessInfo.hThread);
		::CloseHandle(Write);

		if (!WaitDone)
		{
			::CloseHandle(Read);
			::CloseHandle(ProcessInfo.hProcess);
			return;
		}

		std::string Output;
		{
			char Buffer[1024];
			::DWORD Bytes = 0u;

			while (::ReadFile(Read, Buffer, sizeof(Buffer), &Bytes, nullptr) && Bytes > 0u)
			{
				Output.append(Buffer, Bytes);
			}
		}

		::CloseHandle(Read);

		::WaitForSingleObject(ProcessInfo.hProcess, INFINITE);

		::DWORD ExitCode = 0u;
		const bool bGotExitCode = ::GetExitCodeProcess(ProcessInfo.hProcess, &ExitCode) != 0;

		::CloseHandle(ProcessInfo.hProcess);

		if (!bGotExitCode)
		{
			VERIFY_WITH_SYSTEM_MESSAGE(0);
			return;
		}

		if (ExitCode != 0u)
		{
			LOG_ERROR(LogDefault, "Process \"{}\" exited with code {}. {}", Commandline, ExitCode, Output);
		}
	}

	string GetEnvironmentVariable(const char* Name)
	{
		const std::wstring WideName = Utf8ToWide(Name);
		if (WideName.empty())
		{
			return string();
		}

		std::vector<wchar_t> Buffer(UINT16_MAX, L'\0');
		const ::DWORD Length = ::GetEnvironmentVariableW(WideName.c_str(), Buffer.data(), static_cast<::DWORD>(Buffer.size()));
		if (Length == 0u || Length >= Buffer.size())
		{
			return string();
		}

		return string::from_wide(std::wstring_view(Buffer.data(), Length));
	}

	void* GetApplicationHandle()
	{
		::HMODULE Handle = ::GetModuleHandleW(nullptr);
		VERIFY_WITH_SYSTEM_MESSAGE(Handle);
		return reinterpret_cast<void*>(Handle);
	}

	Math::Vector2 GetCursorPosition()
	{
		::POINT Pos{};
		if (!::GetCursorPos(&Pos))
		{
			return Math::Vector2(0.0f, 0.0f);
		}

		return Math::Vector2(static_cast<float>(Pos.x), static_cast<float>(Pos.y));
	}

	uint32_t GetNumHardwareConcurrencyThreads(bool UseHyperThreading)
	{
		std::unique_ptr<uint8_t[]> Buffer;
		::DWORD BufferSize = 0;
		uint32_t PhysicalCoreCount = 0u;
		uint32_t LogicalCoreCount = 0u;

		if (!::GetLogicalProcessorInformationEx(::LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll, reinterpret_cast<::PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(Buffer.get()), &BufferSize) &&
			::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			Buffer = std::make_unique<uint8_t[]>(BufferSize);

			if (::GetLogicalProcessorInformationEx(::LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll, reinterpret_cast<::PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(Buffer.get()), &BufferSize))
			{
				uint8_t* BufferPtr = Buffer.get();
				uint8_t* const BufferEnd = Buffer.get() + BufferSize;

				while (BufferPtr + sizeof(::SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX) <= BufferEnd)
				{
					::PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX ProcessorInfo = reinterpret_cast<::PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(BufferPtr);

					if (ProcessorInfo->Size == 0u)
					{
						break;
					}

					if (ProcessorInfo->Relationship == ::LOGICAL_PROCESSOR_RELATIONSHIP::RelationProcessorCore)
					{
						++PhysicalCoreCount;

						for (uint32_t Index = 0u; Index < ProcessorInfo->Processor.GroupCount; ++Index)
						{
							LogicalCoreCount += static_cast<uint32_t>(std::bitset<sizeof(::KAFFINITY) * CHAR_BIT>(ProcessorInfo->Processor.GroupMask[Index].Mask).count());
						}
					}

					BufferPtr += ProcessorInfo->Size;
				}
			}
		}

		if (PhysicalCoreCount == 0u && LogicalCoreCount == 0u)
		{
			const uint32_t Fallback = std::thread::hardware_concurrency();
			return Fallback > 0u ? Fallback : 1u;
		}

		return UseHyperThreading ? LogicalCoreCount : PhysicalCoreCount;
	}

	void SetThreadPriority(std::thread::id ThreadID, ETFTaskPriority Priority)
	{
		std::stringstream Stream;
		Stream << ThreadID;

		uint32_t NativeThreadID = 0u;
		Stream >> NativeThreadID;

		if (Stream.fail() || NativeThreadID == 0u)
		{
			return;
		}

		::HANDLE ThreadHandle = ::OpenThread(THREAD_SET_INFORMATION, false, static_cast<::DWORD>(NativeThreadID));
		if (!ThreadHandle)
		{
			VERIFY_WITH_SYSTEM_MESSAGE(0);
			return;
		}

		int32_t ThreadPriority = THREAD_PRIORITY_NORMAL;
		switch (Priority)
		{
		case ETFTaskPriority::Critical:
			ThreadPriority = THREAD_PRIORITY_HIGHEST;
			break;
		case ETFTaskPriority::High:
			ThreadPriority = THREAD_PRIORITY_ABOVE_NORMAL;
			break;
		case ETFTaskPriority::Normal:
			ThreadPriority = THREAD_PRIORITY_NORMAL;
			break;
		case ETFTaskPriority::Low:
			ThreadPriority = THREAD_PRIORITY_BELOW_NORMAL;
			break;
		default:
			ThreadPriority = THREAD_PRIORITY_NORMAL;
			break;
		}

		VERIFY_WITH_SYSTEM_MESSAGE(::SetThreadPriority(ThreadHandle, ThreadPriority) != 0);

		::CloseHandle(ThreadHandle);
	}

	std::wstring Utf8ToWide(std::string_view Str)
	{
		if (Str.empty())
		{
			return std::wstring();
		}

		const int32_t Length = ::MultiByteToWideChar(CP_UTF8, 0, Str.data(), static_cast<int32_t>(Str.size()), nullptr, 0);
		if (Length <= 0)
		{
			return std::wstring();
		}

		std::wstring Result(static_cast<size_t>(Length), L'\0');
		::MultiByteToWideChar(CP_UTF8, 0, Str.data(), static_cast<int32_t>(Str.size()), Result.data(), Length);

		return Result;
	}

	string WideToUtf8(std::wstring_view Str)
	{
		if (Str.empty())
		{
			return std::string();
		}

		const int32_t Length = ::WideCharToMultiByte(CP_UTF8, 0, Str.data(), static_cast<int32_t>(Str.size()), nullptr, 0, nullptr, nullptr);
		if (Length <= 0)
		{
			return std::string();
		}

		string Result(static_cast<size_t>(Length), '\0');
		::WideCharToMultiByte(CP_UTF8, 0, Str.data(), static_cast<int32_t>(Str.size()), Result.data(), Length, nullptr, nullptr);

		return Result;
	}

	SharedLibrary::SharedLibrary(const char* LibraryName)
	{
		const std::wstring WideModuleName = PlatformMisc::Utf8ToWide(string::format("{}{}", LibraryName, DLL_EXTENSION));
		m_Handle = reinterpret_cast<void*>(::LoadLibraryW(WideModuleName.c_str()));
		VERIFY_WITH_SYSTEM_MESSAGE(m_Handle);
	}

	void* SharedLibrary::GetProcAddress(const char* FunctionName)
	{
		assert(m_Handle);
		return ::GetProcAddress(reinterpret_cast<::HMODULE>(m_Handle), FunctionName);
	}

	SharedLibrary::~SharedLibrary()
	{
		VERIFY_WITH_SYSTEM_MESSAGE(::FreeLibrary(reinterpret_cast<::HMODULE>(m_Handle)) != 0);
	}
}

#endif // defined(GetEnvironmentVariable)
#endif // PLATFORM_WIN32

