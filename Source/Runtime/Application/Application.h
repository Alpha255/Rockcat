#include "Core/String.h"
#include "Async/TaskEvent.h"

class Application
{
public:
	static Application& Get();

	void Loop();

	virtual void PumpMessages() = 0;

	inline bool IsActive() const { return m_Status == EStatus::Active; }
	inline bool IsQuit() const { return m_Status == EStatus::Quit; }

	//inline class RenderBackend& GetRenderBackend() const { return *m_RenderBackend; }
	virtual std::shared_ptr<class Window> CreateWindow(uint32_t Width, uint32_t Height, const char* Title) = 0;

	void AddMessageHandler(class MessageHandler* Handler);
	void RemoveMessageHandler(class MessageHandler* Handler);

	static std::filesystem::path GetWorkingDirectory();
	static void SetWorkingDirectory(const std::filesystem::path& Directory);

	static void Sleep(uint32_t Seconds);
	static void Sleep(uint32_t Milliseconds);

	static string GetEnvironmentVariable(const char* Name);
	static void SetEnvironmentVariable(const char* Name, const char* Value);

	static const char* GetCommandline();

	static void ExecuteProcess(const char* Commandline, bool WaitDone = true);
	
	static uint32_t GetNumHardwareConcurrencyThreads(bool UseHyperThreading);
	static void SetThreadPriority(std::thread::id ThreadID, ETFTaskPriority Priority);
protected:
	enum class EStatus
	{
		Active,
		Inactive,
		Quit
	};

	inline void SetStatus(EStatus Status) { m_Status = Status; }

	void Initialize();
	void Finalize();

	void Tick(float ElapsedSeconds);
private:
	EStatus m_Status;
	//std::unique_ptr<GameInstance> m_GameInstance;
	//std::unique_ptr<class RenderBackend> m_RenderBackend;
	//std::unique_ptr<class Window> m_Window;

#if WITH_EDITOR
	//std::unique_ptr<class Editor> m_Editor;
#endif
	void* m_Handle = nullptr;
	std::shared_ptr<class Window> m_MainWindow;
};