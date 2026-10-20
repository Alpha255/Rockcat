#include "Core/String.h"
#include "Async/TaskEvent.h"
#include "Misc/MessageHandler.h"

class Application
{
public:
	static Application& Get();

	Application();
	virtual ~Application();

	void Loop();

	virtual void PumpMessages() = 0;

	inline bool IsActive() const { return m_Status == EStatus::Active; }
	inline bool IsQuit() const { return m_Status == EStatus::Quit; }

	inline void* GetHandle() const { return m_Handle; }

	//inline class RenderBackend& GetRenderBackend() const { return *m_RenderBackend; }
	virtual std::shared_ptr<class Window> CreateWindow(uint32_t Width, uint32_t Height, const char* Title) = 0;

	void AddMessageHandler(MessageHandler* Handler);
	void RemoveMessageHandler(MessageHandler* Handler);

	static string GetSystemErrorMessage(uint32_t ErrorCode = std::numeric_limits<uint32_t>().max());

	static std::filesystem::path GetExecutablePath();

	static std::filesystem::path GetWorkingDirectory();
	static void SetWorkingDirectory(const std::filesystem::path& Directory);

	static void Sleep(uint32_t Seconds);

	static string GetEnvironmentVariable(const char* Name);
	static void SetEnvironmentVariable(const char* Name, const char* Value);

	static string GetCommandline();

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

	inline std::shared_ptr<class Window> GetMainWindow() { return m_MainWindow; }

	void Initialize();
	void Finalize();

	void Tick(float ElapsedSeconds);

	void DispatchKeyUpMessage(const KeyEvent& Event);
	void DispatchKeyDownMessage(const KeyEvent& Event);
	void DispatchKeyCharMessage(const char Character);

	void DispatchMouseButtonUpMessage(const EMouseButton Button);
	void DispatchMouseButtonDownMessage(const EMouseButton Button, const Math::Vector2& CursorPos);
	void DispatchMouseButtonDoubleClickMessage(const EMouseButton Button, const Math::Vector2& CursorPos);
	void DispatchMouseMoveMessage(const Math::Vector2& CursorPos);
	void DispatchMouseWheelMessage(float WheelDelta, const Math::Vector2& CursorPos);

	void DispatchMainWindowResizedMessage(uint32_t Width, uint32_t Height);
	void DispatchAppActiveChangedMessage();
	void DispatchAppInactiveChangedMessage();
	void DispatchAppQuitMessage();

	virtual EKeyboardKey DecodeKeyboardKey(uint64_t KeyCode, int64_t Mask) = 0;

	KeyModifiers m_KeyModifiers;
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
	std::vector<MessageHandler*> m_MessageHandlers;
};