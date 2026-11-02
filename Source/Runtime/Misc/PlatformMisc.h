#pragma once

#include "Core/Math/Vector2.h"
#include "Async/TaskEvent.h"

namespace PlatformMisc
{
	std::wstring Utf8ToWide(std::string_view Str);

	string WideToUtf8(std::wstring_view Str);

	string GetSystemErrorMessage(uint32_t ErrorCode);

	std::filesystem::path GetExecutablePath();

	std::filesystem::path GetWorkingDirectory();

	void SetWorkingDirectory(const std::filesystem::path& Directory);

	void Sleep(uint32_t Seconds);

	void ExecuteProcess(const char* Commandline, bool WaitDone);

	string GetEnvironmentVariable(const char* Name);

	void SetEnvironmentVariable(const char* Name, const char* Value);

	void* GetApplicationHandle();

	const string GetCommandline();

	Math::Vector2 GetCursorPosition();

	uint32_t GetNumHardwareConcurrencyThreads(bool UseHyperThreading);

	void SetThreadPriority(std::thread::id ThreadID, ETFTaskPriority Priority);
}

