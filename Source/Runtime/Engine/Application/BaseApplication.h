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
	
	bool IsActivate() const;

	const Viewport& GetRenderViewport() { return *m_RenderViewport; }
	class RHIBackend& GetRenderBackend();
	const struct RenderSettings& GetRenderSettings() const;
protected:
	friend class Engine;

	void InitializeRHI();

	virtual void RenderGUI(class Canvas&) {}
private:
	std::shared_ptr<struct ApplicationConfiguration> m_Configs;
	std::unique_ptr<Viewport> m_RenderViewport;
	std::unique_ptr<class RHIBackend> m_RenderBackend;
};