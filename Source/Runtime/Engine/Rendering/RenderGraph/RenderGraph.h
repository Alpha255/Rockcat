#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass.h"
#include "Core/MessageRouter.h"

class RenderGraph : public MessageHandler
{
public:
	static std::shared_ptr<RenderGraph> Create(class RHIDevice& Device, const struct RenderSettings& GraphicsSettings, const class Viewport& RenderViewport);

	RenderGraph(class RHIDevice& Device, const struct RenderSettings& GraphicsSettings, const class Viewport& RenderViewport);

	template<class TPass>
	RenderGraph& AddPass()
	{
		m_RenderPasses.emplace_back(std::make_shared<TPass>(m_Graph.AddNode(), *this));
		return *this;
	}

	void Execute(const class Scene& InScene);

	const struct RenderSettings& GetRenderSettings() const { return m_RenderSettings; }

	class RHIDevice& GetRenderDevice() { return m_RenderDevice; }
	RenderScene& GetRenderScene() { return *m_RenderScene; }
	const RenderScene& GetRenderScene() const { return *m_RenderScene; }

	class ResourceManager& GetResourceManager() { return *m_ResourceMgr; }

	Math::UInt2 GetDisplaySize() const;

	void OnWindowResized(uint32_t Width, uint32_t Height) override final;
protected:
	virtual void SetupRenderPasses() = 0;
private:
	inline void SetDirty(bool Dirty) { m_Dirty = Dirty; }
	void Compile();

	bool m_Dirty = true;
	DirectedAcyclicGraph m_Graph;
	std::shared_ptr<class ResourceManager> m_ResourceMgr;
	std::shared_ptr<RenderScene> m_RenderScene;
	std::vector<std::shared_ptr<RenderPass>> m_RenderPasses;
	class RHIDevice& m_RenderDevice;
	const struct RenderSettings& m_RenderSettings;
	const class Viewport& m_RenderViewport;
};
