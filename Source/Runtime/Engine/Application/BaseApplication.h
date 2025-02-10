#pragma once

#include "Engine/Tickable.h"
#include "Engine/Application/Viewport.h"

class BaseApplication : public NoneCopyable, public ITickable
{
public:
	BaseApplication(const char* ConfigurationPath);

	virtual ~BaseApplication() = default;

	virtual void Initialize() {}
	virtual void RenderFrame() {}
	virtual bool IsRequestQuit() const;
	virtual void PumpMessages();
	virtual void Finalize() {}

	void Tick(float /*ElapsedSeconds*/) override {}
	const Viewport& GetRenderViewport() { return *m_RenderViewport; }
	class RHIBackend& GetRenderBackend();
	const struct RenderSettings& GetRenderSettings() const;
protected:
	friend class RenderService;

	virtual void RenderGUI(class Canvas&) {}

	void SetRenderBackend(class RHIBackend* Backend);
private:
	std::shared_ptr<struct ApplicationConfiguration> m_Configs;
	std::unique_ptr<Viewport> m_RenderViewport;
	class RHIBackend* m_RenderBackend = nullptr;
};