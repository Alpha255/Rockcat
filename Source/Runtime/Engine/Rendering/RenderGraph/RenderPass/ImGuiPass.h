#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass.h"

class ImGUIPass : public RenderPass
{
public:
	DECLARE_RENDERPASS_CONSTRUCTOR(ImGUIPass, RenderPass)

	~ImGUIPass();

	void Execute(class RHIDevice&, const class Scene&) override final {}
protected:
private:
	struct ImGuiContext* m_Context = nullptr;
};