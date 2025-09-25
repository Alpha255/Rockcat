#pragma once

#include "Core/GUID.h"
#include "Core/Math/Vector2.h"
#include "Async/Task.h"

class System
{
public:
	static std::string GetErrorMessage(uint32_t ErrorCode = ~0u);

	static std::filesystem::path GetWorkingDirectory();
	static void SetWorkingDirectory(const std::filesystem::path& Directory);

	static void Sleep(uint32_t Milliseconds);

	static std::string GetEnvironmentVariables(const char* Variable);

	static std::filesystem::path GetApplicationDirectory();
	static void* GetApplicationInstance();

	static void ExecuteProcess(const char* Commandline, bool WaitDone = true);

	static Guid CreateGUID();

	static Math::Vector2 GetCursorPosition();

	static size_t GetHardwareConcurrencyThreadsCount(bool UseHyperThreading);

	static void SetThreadPriority(std::thread::id ThreadID, Task::EPriority Priority);

	static void PumpMessages();
};

