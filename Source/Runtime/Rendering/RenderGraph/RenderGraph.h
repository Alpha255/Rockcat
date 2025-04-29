#pragma once

#include "Rendering/RenderGraph/RenderPass.h"

class RenderGraph
{
public:
	static std::shared_ptr<RenderGraph> Create(const struct RenderSettings& InRenderSettings, const class IView& InView);

	RenderGraph(const struct RenderSettings& InRenderSettings, const class IView& InView);

	template<class TPass>
	RenderGraph& AddPass()
	{
		m_RenderPasses.emplace_back(std::make_shared<TPass>(m_Graph.AddNode(), *this));
		return *this;
	}

	template<class Setup, class Execute>
	RenderPass& AddPass(const char* Name)
	{
		auto Pass = m_RenderPasses.emplace_back(std::make_shared<RenderPass>(m_Graph.AddNode(), Name, *this));
		return *Pass;
	}

	void Execute(const class Scene& InScene);

	const struct RenderSettings& GetRenderSettings() const { return m_RenderSettings; }
	class ResourceManager& GetResourceManager() { return *m_ResourceMgr; }
protected:
	virtual void SetupRenderPasses() = 0;
private:
	inline void SetDirty(bool Dirty) { m_Dirty = Dirty; }
	
	void Compile();

	void SetDisplaySize(uint32_t Width, uint32_t Height);

	bool m_Dirty = true;
	DirectedAcyclicGraph m_Graph;
	std::shared_ptr<class ResourceManager> m_ResourceMgr;
	std::shared_ptr<RenderScene> m_RenderScene;
	std::vector<std::shared_ptr<RenderPass>> m_RenderPasses;
	class RHIDevice& m_Device;
	const class IView& m_SceneView;
	const struct RenderSettings& m_RenderSettings;
};
