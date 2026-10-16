#pragma once

#include "Core/Definitions.h"

#if PLATFORM_WIN32
	#include "Application/Win32/WindowsApplication.h"
#elif PLATFORM_LINUX
	#include "Application/Linux/LinuxApplication.h"
#else
	#error Unknown platform
#endif

class GameInstance
{
public:
	GameInstance(const char* Name)
		: m_Name(Name)
	{
	}

	inline std::string_view GetName() const { return m_Name; }

	virtual void OnBeginPlay() {}

	virtual void Tick(float ElapsedSeconds) {}

	virtual void OnEndPlay() {}

	void OnAttachToViewport(class Viewport& Viewport);
private:
	std::string_view m_Name;
};

class GenericPlatformApplication
{
public:
	static GenericPlatformApplication& Get();

	void SetWindowTitle(const char* Title);

	void Loop();

	void PumpMessages();

	inline class RenderBackend& GetRenderBackend() const { return *m_RenderBackend; }
protected:
	enum class EStatus
	{
		Active,
		Inactive,
		Quit
	};

	inline void SetStatus(EStatus Status) { m_Status = Status; }
	inline bool IsActive() const { return m_Status == EStatus::Active; }
	inline bool IsQuit() const { return m_Status == EStatus::Quit; }

	void CreateMainWindow(uint32_t Width, uint32_t Height);

	void Initialize();
	void Finalize();

	void Tick(float ElapsedSeconds);
private:
	EStatus m_Status;
	std::unique_ptr<GameInstance> m_GameInstance;
	std::unique_ptr<class RenderBackend> m_RenderBackend;
	std::unique_ptr<class Window> m_Window;

#if WITH_EDITOR
	std::unique_ptr<class Editor> m_Editor;
#endif
};
