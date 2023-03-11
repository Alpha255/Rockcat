#pragma once

#include "Colorful/IRenderer/IPipeline.h"
#include "Colorful/IRenderer/RenderSettings.h"
#include "Colorful/IRenderer/ICommandBuffer.h"

class MeshInstance;
class Camera;
class SceneNode;
class Scene;
class Material;

NAMESPACE_START(RHI)

struct Drawable
{
	const MeshInstance* Mesh = nullptr;
	std::vector<const SceneNode*> m_Instances;
};

class IFrameGraphPass
{
public:
	IFrameGraphPass(const Scene* TargetScene, const Camera* ViewCamera)
		: m_Scene(TargetScene)
		, m_Camera(ViewCamera)
	{
		assert(TargetScene && ViewCamera);
	}

	virtual ~IFrameGraphPass() = default;

	virtual void ApplyRenderSettings(const RenderSettings* Settings)
	{
		m_Viewport = Viewport(static_cast<float32_t>(Settings->Resolution.Width), static_cast<float32_t>(Settings->Resolution.Height));
		m_Scissor = ScissorRect(static_cast<float32_t>(Settings->Resolution.Width), static_cast<float32_t>(Settings->Resolution.Height));
	}

	virtual void Render(ICommandBuffer* /*Command*/) {}

	virtual void SetupMaterial(const MeshInstance* /*Mesh*/) {}

	virtual void UpdateUniformBuffers(ICommandBuffer* /*Command*/) {}

	void Draw(class ICommandBuffer* Command, const Drawable& TargetDrawable);

	void SetSwapchainFrameBuffer(IFrameBuffer* SwapchainFrameBuffer)
	{
		m_SwapchainFrameBuffer = SwapchainFrameBuffer;
	}

	void BindPass(IFrameGraphPass* PassToBind)
	{
		if (PassToBind != this)
		{
			m_BindPass = PassToBind;
		}
	}

	bool8_t Enabled() const
	{
		return m_Enable;
	}
protected:
	void SetEnable(bool8_t Enabled)
	{
		m_Enable = Enabled;
	}

	const Scene* m_Scene = nullptr;
	const Camera* m_Camera = nullptr;
	IFrameGraphPass* m_BindPass = nullptr;
	IFrameBuffer* m_SwapchainFrameBuffer = nullptr;

	IFrameBufferPtr m_FrameBuffer;
	IPipelinePtr m_GraphicsPipeline;
	GraphicsPipelineDesc m_GraphicsPipelineDesc;

	Viewport m_Viewport;
	ScissorRect m_Scissor;
private:
	bool8_t m_Enable = true;
};

class OpaquePass : public IFrameGraphPass
{
public:
	using IFrameGraphPass::IFrameGraphPass;
protected:
	Frustum m_Frustum;
	bool8_t m_EnableFrustumCulling = true;
private:
};

class IFrameGraph
{
public:
	IFrameGraph(ERenderingPath RenderingPath, IFrameGraph* LastGraph);

	virtual ~IFrameGraph() = default;

	virtual void Render(const RenderSettings* Settings, IFrameBuffer* SwapchainFrameBuffer) = 0;

	template<class Pass>
	Pass* AddPass(const Scene* TargetScene, Camera* ViewCamera)
	{
		m_Passes.emplace_back(std::make_unique<Pass>(TargetScene, ViewCamera));
		return static_cast<Pass*>(m_Passes.back().get());
	}

	ERenderingPath RenderingPath() const
	{
		return m_RenderingPath;
	}
protected:
	ERenderingPath m_RenderingPath = ERenderingPath::ForwardShading;
	std::vector<std::unique_ptr<IFrameGraphPass>> m_Passes;
	IDevice* m_Device = nullptr;
};

NAMESPACE_END(RHI)
