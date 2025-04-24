#pragma once

#include "Application/RunApplication.h"
#include "Core/Tickable.h"

class BaseApplication : public NoneCopyable, public ITickable
{
public:
	BaseApplication(const char* ConfigurationPath);

	virtual ~BaseApplication();

	virtual void Initialize();
	virtual void RenderFrame() {}
	virtual void Finalize() {}
	
	void Tick(float) override {}
	
	virtual void PumpMessages();

	inline bool IsActivate() const { return m_Activate; }
	inline bool IsRequestQuit() const { return m_RequestQuit; }

	inline const class Window& GetWindow() const { return *m_Window; }
	inline const struct ApplicationConfiguration& GetConfigs() const { return *m_Configs; }
	const struct RenderSettings& GetRenderSettings() const;

	void ProcessMessage(uint32_t Message, size_t WParam, intptr_t LParam);
protected:
	virtual void RenderGUI(class Canvas&) {}
	virtual void InitializeImpl() {}
private:
	std::shared_ptr<struct ApplicationConfiguration> m_Configs;
	std::unique_ptr<class Window> m_Window;
	bool m_Activate = true;
	bool m_RequestQuit = false;
};