#pragma once

#include "Core/Tickable.h"

class BaseApplication : public NoneCopyable, public ITickable
{
public:
	BaseApplication(const char* ConfigPath);
	virtual ~BaseApplication() = default;

	virtual void Initialize() {}
	virtual void Render(class RHITexture*) {}
	virtual void RenderGUI() {}
	virtual void Finalize() {}
	
	virtual void PumpMessages();

	virtual void Run();
	
	void Tick(float) override {}

	bool IsActivate() const;
	bool IsRequestQuit() const;
protected:
	bool InitializeRHI();
	void Present();
private:
	std::unique_ptr<class Window> m_Window;
	std::shared_ptr<struct ApplicationConfiguration> m_Configs;

	std::unique_ptr<class RHIBackend> m_RenderBackend;
	std::shared_ptr<class RHISwapchain> m_RenderSwapchain;

	std::unique_ptr<class CpuTimer> m_Timer;
};