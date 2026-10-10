#pragma once

#include "Core/Math/Vector2.h"
#include "Core/String.h"
#include "Async/TaskEvent.h"

namespace PlatformMisc
{
	string GetErrorMessage(uint32_t ErrorCode = ~0u);

	std::filesystem::path GetWorkingDirectory();
	void SetWorkingDirectory(const std::filesystem::path& Directory);

	void Sleep(uint32_t Seconds);

	string GetEnvironmentVariable(const char* Name);

	std::filesystem::path GetApplicationPath();
	void* GetApplicationHandle();

	void ExecuteProcess(const char* Commandline, bool WaitDone = true);

	Math::Vector2 GetCursorPosition();

	uint32_t GetNumHardwareConcurrencyThreads(bool UseHyperThreading);

	void SetThreadPriority(std::thread::id ThreadID, ETFTaskPriority Priority);

	std::wstring Utf8ToWide(std::string_view Str);
	string WideToUtf8(std::wstring_view Str);

	class SharedLibrary
	{
	public:
		SharedLibrary(const char* LibraryName);
		virtual ~SharedLibrary();

		void* GetProcAddress(const char* FunctionName);
	protected:
	private:
		void* m_Handle = nullptr;
	};
};

