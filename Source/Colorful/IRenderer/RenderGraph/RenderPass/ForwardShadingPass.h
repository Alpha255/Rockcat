#pragma once

#include "Colorful/IRenderer/RenderGraph/IFrameGraph.h"

NAMESPACE_START(RHI)

class ForwardShadingPass : public OpaquePass
{
public:
	ForwardShadingPass(const Scene* TargetScene, const Camera* ViewCamera);

	void ApplyRenderSettings(const RenderSettings*) override final;
	void Render(class ICommandBuffer*) override final;
	void SetupMaterial(const MeshInstance* Mesh) override final;
	void UpdateUniformBuffers(class ICommandBuffer* Command) override final;

	IImagePtr GetColorAttachment() const
	{
		return m_FrameBuffer->Description().ColorAttachments[0];
	}
protected:
	bool8_t BatchDrawable(const SceneNode* Object);
private:
	std::vector<Drawable> m_Drawables;
	EShadowTechnique m_ShadowTechnique = EShadowTechnique::None;
	const Material* m_LastMaterial = nullptr;
	ISamplerPtr m_LinearSampler;
	IBufferPtr m_Transform;
	IBufferPtr m_Lighting;
};

NAMESPACE_END(RHI)
