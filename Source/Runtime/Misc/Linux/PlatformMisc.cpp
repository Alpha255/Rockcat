#include "Misc/PlatformMisc.h"
#include "Core/SpdLogging.h"

#if PLATFORM_LINUX

#include <cerrno>
#include <cstdlib>
#include <dlfcn.h>
#include <fstream>
#include <limits.h>
#include <set>
#include <system_error>
#include <time.h>
#include <unistd.h>

#include <sys/resource.h>
#include <sys/wait.h>

namespace PlatformMisc
{
	string GetErrorMessage(uint32_t ErrorCode)
	{
		const int32_t Code = ErrorCode == ~0u ? errno : static_cast<int32_t>(ErrorCode);
		return string(std::generic_category().message(Code));
	}

	std::filesystem::path GetApplicationPath()
	{
		std::vector<char> Buffer(static_cast<size_t>(PATH_MAX), '\0');

		const ::ssize_t Length = ::readlink("/proc/self/exe", Buffer.data(), Buffer.size() - 1u);
		if (Length <= 0)
		{
			VERIFY_WITH_SYSTEM_MESSAGE(0);
			return std::filesystem::path();
		}

		Buffer[static_cast<size_t>(Length)] = '\0';
		return std::filesystem::path(Buffer.data());
	}

	std::filesystem::path GetWorkingDirectory()
	{
		std::error_code Error;
		const std::filesystem::path Path = std::filesystem::current_path(Error);
		if (Error)
		{
			VERIFY_WITH_SYSTEM_MESSAGE(0);
			return std::filesystem::path();
		}

		return Path;
	}

	void SetWorkingDirectory(const std::filesystem::path& Directory)
	{
		assert(std::filesystem::exists(Directory));

		std::error_code Error;
		std::filesystem::current_path(Directory, Error);
		if (Error)
		{
			VERIFY_WITH_SYSTEM_MESSAGE(0);
		}
	}

	void Sleep(uint32_t Seconds)
	{
		::timespec Request{};
		Request.tv_sec = static_cast<::time_t>(Seconds);

		while (::nanosleep(&Request, &Request) != 0 && errno == EINTR)
		{
		}
	}

	void ExecuteProcess(const char* Commandline, bool WaitDone)
	{
		if (!Commandline || !*Commandline)
		{
			return;
		}

		int32_t PipeDescriptors[2] = { -1, -1 };
		if (::pipe(PipeDescriptors) != 0)
		{
			VERIFY_WITH_SYSTEM_MESSAGE(0);
			return;
		}

		const ::pid_t Child = ::fork();
		if (Child < 0)
		{
			::close(PipeDescriptors[0]);
			::close(PipeDescriptors[1]);
			VERIFY_WITH_SYSTEM_MESSAGE(0);
			return;
		}

		if (Child == 0)
		{
			::close(PipeDescriptors[0]);
			::dup2(PipeDescriptors[1], STDOUT_FILENO);
			::dup2(PipeDescriptors[1], STDERR_FILENO);
			::close(PipeDescriptors[1]);

			::execl("/bin/sh", "sh", "-c", Commandline, static_cast<char*>(nullptr));
			::_exit(127);
		}

		::close(PipeDescriptors[1]);

		if (!WaitDone)
		{
			::close(PipeDescriptors[0]);
			return;
		}

		std::string Output;
		{
			char Buffer[1024];
			::ssize_t Bytes = 0;

			while ((Bytes = ::read(PipeDescriptors[0], Buffer, sizeof(Buffer))) > 0)
			{
				Output.append(Buffer, static_cast<size_t>(Bytes));
			}
		}

		::close(PipeDescriptors[0]);

		int32_t Status = 0;
		::waitpid(Child, &Status, 0);

		const int32_t ExitCode = WIFEXITED(Status) ? static_cast<int32_t>(WEXITSTATUS(Status)) : -1;

		if (ExitCode != 0)
		{
			LOG_ERROR(LogDefault, "Process \"{}\" exited with code {}. {}", Commandline, ExitCode, Output);
		}
	}

	string GetEnvironmentVariable(const char* Name)
	{
		if (!Name || !*Name)
		{
			return string();
		}

		const char* const Value = std::getenv(Name);
		return Value ? string(Value) : string();
	}

	void* GetApplicationHandle()
	{
		return ::dlopen(nullptr, RTLD_NOW);
	}

	Math::Vector2 GetCursorPosition()
	{
		return Math::Vector2(0.0f, 0.0f);
	}

	size_t GetNumHardwareConcurrencyThreads(bool UseHyperThreading)
	{
		const size_t LogicalCoreCount = std::thread::hardware_concurrency();

		if (UseHyperThreading || LogicalCoreCount == 0u)
		{
			return LogicalCoreCount > 0u ? LogicalCoreCount : 1u;
		}

		std::ifstream CpuInfo("/proc/cpuinfo");
		if (!CpuInfo)
		{
			return LogicalCoreCount;
		}

		std::set<std::pair<int32_t, int32_t>> Cores;
		int32_t PhysicalID = -1;
		int32_t CoreID = -1;
		std::string Line;

		while (std::getline(CpuInfo, Line))
		{
			if (Line.empty())
			{
				if (PhysicalID >= 0 && CoreID >= 0)
				{
					Cores.emplace(PhysicalID, CoreID);
				}

				PhysicalID = -1;
				CoreID = -1;
				continue;
			}

			const size_t Separator = Line.find(':');
			if (Separator == std::string::npos)
			{
				continue;
			}

			const std::string Key = Line.substr(0u, Separator);

			if (Key.find("physical id") != std::string::npos)
			{
				PhysicalID = std::atoi(Line.c_str() + Separator + 1u);
			}
			else if (Key.find("core id") != std::string::npos)
			{
				CoreID = std::atoi(Line.c_str() + Separator + 1u);
			}
		}

		if (PhysicalID >= 0 && CoreID >= 0)
		{
			Cores.emplace(PhysicalID, CoreID);
		}

		return Cores.empty() ? LogicalCoreCount : Cores.size();
	}

	void SetThreadPriority(std::thread::id ThreadID, TFTask::EPriority Priority)
	{
		if (ThreadID != std::this_thread::get_id())
		{
			return;
		}

		int32_t NiceValue = 0;
		switch (Priority)
		{
		case TFTask::EPriority::Critical:
			NiceValue = -10;
			break;
		case TFTask::EPriority::High:
			NiceValue = -5;
			break;
		case TFTask::EPriority::Normal:
			NiceValue = 0;
			break;
		case TFTask::EPriority::Low:
			NiceValue = 5;
			break;
		default:
			NiceValue = 0;
			break;
		}

		::setpriority(PRIO_PROCESS, 0, NiceValue);
	}

	std::wstring Utf8ToWide(std::string_view Str)
	{
		std::wstring Result;
		Result.reserve(Str.size());

		const uint8_t* Ptr = reinterpret_cast<const uint8_t*>(Str.data());
		const uint8_t* const End = Ptr + Str.size();

		while (Ptr < End)
		{
			const uint8_t Lead = *Ptr++;
			uint32_t Code = 0u;
			size_t Extra = 0u;

			if (Lead < 0x80u)
			{
				Code = Lead;
			}
			else if ((Lead & 0xE0u) == 0xC0u)
			{
				Code = Lead & 0x1Fu;
				Extra = 1u;
			}
			else if ((Lead & 0xF0u) == 0xE0u)
			{
				Code = Lead & 0x0Fu;
				Extra = 2u;
			}
			else if ((Lead & 0xF8u) == 0xF0u)
			{
				Code = Lead & 0x07u;
				Extra = 3u;
			}
			else
			{
				Result.push_back(static_cast<wchar_t>(0xFFFDu));
				continue;
			}

			size_t Consumed = 0u;
			bool bValid = true;

			for (size_t Index = 0u; Index < Extra && Ptr + Index < End; ++Index)
			{
				const uint8_t Trail = Ptr[Index];
				if ((Trail & 0xC0u) != 0x80u)
				{
					bValid = false;
					break;
				}

				Code = (Code << 6) | (Trail & 0x3Fu);
				++Consumed;
			}

			Ptr += Consumed;

			static const uint32_t MinCode[4] = { 0u, 0x80u, 0x800u, 0x10000u };

			if (!bValid || Code > 0x10FFFFu || Code < MinCode[Extra] || (Code >= 0xD800u && Code <= 0xDFFFu))
			{
				Result.push_back(static_cast<wchar_t>(0xFFFDu));
				continue;
			}

			Result.push_back(static_cast<wchar_t>(Code));
		}

		return Result;
	}

	string WideToUtf8(std::wstring_view Str)
	{
		string Result;
		Result.reserve(Str.size() * 4u);

		for (const wchar_t WideChar : Str)
		{
			uint32_t Code = static_cast<uint32_t>(WideChar);

			if (Code > 0x10FFFFu || (Code >= 0xD800u && Code <= 0xDFFFu))
			{
				Code = 0xFFFDu;
			}

			if (Code < 0x80u)
			{
				Result.push_back(static_cast<char>(Code));
			}
			else if (Code < 0x800u)
			{
				Result.push_back(static_cast<char>(0xC0u | (Code >> 6)));
				Result.push_back(static_cast<char>(0x80u | (Code & 0x3Fu)));
			}
			else if (Code < 0x10000u)
			{
				Result.push_back(static_cast<char>(0xE0u | (Code >> 12)));
				Result.push_back(static_cast<char>(0x80u | ((Code >> 6) & 0x3Fu)));
				Result.push_back(static_cast<char>(0x80u | (Code & 0x3Fu)));
			}
			else
			{
				Result.push_back(static_cast<char>(0xF0u | (Code >> 18)));
				Result.push_back(static_cast<char>(0x80u | ((Code >> 12) & 0x3Fu)));
				Result.push_back(static_cast<char>(0x80u | ((Code >> 6) & 0x3Fu)));
				Result.push_back(static_cast<char>(0x80u | (Code & 0x3Fu)));
			}
		}

		return Result;
	}

	SharedLibrary::SharedLibrary(const char* LibraryName)
	{
		const string LibraryPath = string::format("{}{}", LibraryName, ".so");
		m_Handle = ::dlopen(LibraryPath.c_str(), RTLD_NOW | RTLD_LOCAL);
		VERIFY_WITH_SYSTEM_MESSAGE(m_Handle);
	}

	void* SharedLibrary::GetProcAddress(const char* FunctionName)
	{
		assert(m_Handle);
		return ::dlsym(m_Handle, FunctionName);
	}

	SharedLibrary::~SharedLibrary()
	{
		VERIFY_WITH_SYSTEM_MESSAGE(::dlclose(m_Handle) == 0);
	}
}

#endif // PLATFORM_LINUX
