#pragma once

#include "Editor/ImGuiConfigurations.h"

class ImGuiEditor
{
public:
	ImGuiEditor();
protected:
private:
	std::shared_ptr<ImGuiConfigurations> m_Configs;
};