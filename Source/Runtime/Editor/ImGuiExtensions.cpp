#include "Editor/ImGuiExtensions.h"
#include "Editor/ImGuiConfigurations.h"

namespace ImGui
{
	void DrawItemActivityOutline(float Rounding, bool DrawWhenInactive, const Math::Color& ColorWhenActive)
	{
		auto* DrawList = ImGui::GetWindowDrawList();
		const ImRect ExpandedRect(ImGui::GetItemRectMin() - 1.0f, ImGui::GetItemRectMax() + 1.0f);
		
		if (ImGui::IsItemHovered() && !ImGui::IsItemActive())
		{
			DrawList->AddRect(ExpandedRect.Min, ExpandedRect.Max, ImColor(60, 60, 60), Rounding, 0, 1.5f);
		}
		
		if (ImGui::IsItemActive())
		{
			DrawList->AddRect(ExpandedRect.Min, ExpandedRect.Max, ColorWhenActive.RGBA8(), Rounding, 0, 1.0f);
		}
		else if (!ImGui::IsItemHovered() && DrawWhenInactive)
		{
			DrawList->AddRect(ExpandedRect.Min, ExpandedRect.Max, ImColor(50, 50, 50), Rounding, 0, 1.0f);
		}
	}

	void DrawToolTip(const char* Tip)
	{
		IMGUI_SCOPED_STYLE(ImGuiStyleVar_WindowPadding, Math::Vector2(5.0f, 5.0f));

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(Tip);
			ImGui::EndTooltip();
		}
	}
}