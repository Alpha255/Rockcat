#pragma once

#include "Core/GUID.h"
#include "Core/Math/Vector2.h"
#include "Async/Task.h"

namespace OS
{
	std::string GetErrorMessage(uint32_t ErrorCode = ~0u);

	std::filesystem::path GetWorkingDirectory();
	void SetWorkingDirectory(const std::filesystem::path& Directory);

	void Sleep(uint32_t Milliseconds);

	std::string GetEnvironmentVariables(const char* Variable);

	std::filesystem::path GetApplicationDirectory();
	void* GetApplicationInstance();

	void ExecuteProcess(const char* Commandline, bool WaitDone = true);

	Guid CreateGUID();

	Math::Vector2 GetCursorPosition();

	size_t GetHardwareConcurrencyThreadsCount(bool UseHyperThreading);

	void SetThreadPriority(std::thread::id ThreadID, TFTask::EPriority Priority);
};

