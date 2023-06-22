#pragma once

#include "Runtime/Core/GUID.h"
#include "Runtime/Core/Math/Vector2.h"

class PlatformMisc
{
public:
	static std::string GetErrorMessage(uint32_t ErrorCode = ~0u);

	static std::filesystem::path GetCurrentWorkingDirectory();

	static void SetCurrentWorkingDirectory(const std::filesystem::path& Directory);

	static void Sleep(uint32_t Milliseconds);

	static std::string GetEnvironmentVariables(const char8_t* Variable);

	static std::filesystem::path GetCurrentModuleDirectory();

	static void* GetCurrentModuleHandle();

	static void ExecuteProcess(const char8_t* Commandline, bool8_t WaitDone = true);

	static Guid CreateGUID();

	static Math::Vector2 GetCurrentCursorPosition();

	static size_t GetHardwareConcurrencyThreadsCount(bool8_t UseHyperThreading);
};

