#pragma once

#include "Colorful/IRenderer/RenderGraph/IFrameGraph.h"

struct ImGuiContext;

NAMESPACE_START(RHI)

class ImGUIPass : public IFrameGraphPass, public IInputHandler, public ITickable
{
public:
	ImGUIPass(const Scene* TargetScene, const Camera* ViewCamera);
	
	~ImGUIPass();

	using OnBuildImGUIWidgets = std::function<void()>;

	void ApplyRenderSettings(const RenderSettings* Settings) override final;

	void Render(class ICommandBuffer* Command) override final;

	void BindBuildWidgetsFunc(OnBuildImGUIWidgets Func)
	{
		m_BuildWidgetsFunc = Func;
	}

	void OnMouseEvent(const MouseEvent& Mouse) override final;

	void OnKeyboardEvent(const KeyboardEvent& Keyboard) override final;

	void OnWindowResized(uint32_t Width, uint32_t Height) override final;

	void Tick(float32_t ElapsedSeconds) override final;

	bool8_t WantCaptureInput() const;
protected:
	void FlushDrawData(class ICommandBuffer* Command);
	void CustomStyles();
private:
	ImGuiContext* m_Context = nullptr;
	IBufferPtr m_VertexBuffer;
	IBufferPtr m_IndexBuffer;
	IBufferPtr m_UniformBuffer;
	IPipelinePtr m_GraphicsPipeline;
	IInputLayoutPtr m_InputLayout;
	IImagePtr m_FontImage;
	ISamplerPtr m_LinearSampler;
	IDevice* m_Device = nullptr;
	std::shared_ptr<Material> m_Material;
	GraphicsPipelineDesc m_GraphicsPipelineDesc;
	OnBuildImGUIWidgets m_BuildWidgetsFunc;

	int32_t m_LastVertexCount = 0;
	int32_t m_LastIndexCount = 0;
};

NAMESPACE_END(RHI)