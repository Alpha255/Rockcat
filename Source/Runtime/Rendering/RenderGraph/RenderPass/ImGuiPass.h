#pragma once

#include "Rendering/RenderGraph/RenderPass.h"

class ImGUIPass : public RenderPass
{
public:
	~ImGUIPass();

	void Compile() override final {}

	void Execute(const RenderScene&) override final {}
protected:
private:
	struct ImGuiContext* m_Context = nullptr;
};