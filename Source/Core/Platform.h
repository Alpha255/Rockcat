#pragma once

#include "Core/GUID.h"
#include "Core/Math/Vector2.h"

NAMESPACE_START(Gear)

namespace Platform
{
	std::string GetErrorMessage(uint32_t ErrCode = ~0u);

	std::string GetCurrentWorkingDirectory();

	void SetCurrentWorkingDirectory(const char8_t* Path);

	void Sleep(uint32_t Milliseconds);

	std::string GetEnvironmentVariables(const char8_t* Var);

	std::string GetCurrentModuleDirectory();

	uint64_t GetCurrentModuleHandle();

	void ExecuteProcess(const char8_t* Commandline, bool8_t WaitDone = true);

	Guid CreateGUID();

	Math::Vector2 GetCurrentCursorPosition();

	class ScopeWorkingDirectory
	{
	public:
		ScopeWorkingDirectory(const char8_t* Path)
		{
			m_LastWorkingDirectory = Platform::GetCurrentWorkingDirectory();
			Platform::SetCurrentWorkingDirectory(Path);
		}

		~ScopeWorkingDirectory()
		{
			Platform::SetCurrentWorkingDirectory(m_LastWorkingDirectory.c_str());
		}

		const char8_t* LastWorkingDirectory() const
		{
			return m_LastWorkingDirectory.c_str();
		}
	protected:
	private:
		std::string m_LastWorkingDirectory;
	};

	class DynamicLibrary
	{
	public:
		DynamicLibrary(const char8_t* Name);
		virtual ~DynamicLibrary();

		void* GetProcAddress(const char8_t* Name);
	protected:
	private:
		uint64_t m_Handle = 0u;
	};
};

NAMESPACE_END(Gear)
