#pragma once

#include "Core/GUID.h"
#include "Core/Math/Vector2.h"

class PlatformMisc
{
public:
	static std::string GetErrorMessage(uint32_t ErrorCode = ~0u);

	static std::filesystem::path GetCurrentWorkingDirectory();

	static void SetCurrentWorkingDirectory(const std::filesystem::path& Directory);

	static void Sleep(uint32_t Milliseconds);

	static std::string GetEnvironmentVariables(const char* Variable);

	static std::filesystem::path GetCurrentModuleDirectory();

	static void* GetCurrentModuleHandle();

	static void ExecuteProcess(const char* Commandline, bool WaitDone = true);

	static Guid CreateGUID();

	static Math::Vector2 GetCurrentCursorPosition();

	static size_t GetHardwareConcurrencyThreadsCount(bool UseHyperThreading);
};

