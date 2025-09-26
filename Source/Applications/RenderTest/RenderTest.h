#pragma once

#include "Application/PlatformApplication.h"

class RenderTest final : public PlatformApplication
{
public:
	using PlatformApplication::PlatformApplication;

	void Initialize() override final;
	void Render() override final;
protected:
private:
	std::shared_ptr<class Scene> m_Scene;
	std::shared_ptr<class RenderGraph> m_RenderGraph;
};