#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass.h"

class ImGUIPass : public RenderPass
{
public:
	DECLARE_RENDERPASS_CONSTRUCTOR(ImGUIPass, RenderPass)

	~ImGUIPass();

	void Compile() override final {}

	void Execute(class RHIDevice&, const RenderScene&) override final {}
protected:
private:
	struct ImGuiContext* m_Context = nullptr;
};