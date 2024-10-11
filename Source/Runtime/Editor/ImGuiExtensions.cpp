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

	std::string AddLabelStressline(const char* Label)
	{
		std::string NewLabel;
		ImGuiWindow* Window = GetCurrentWindow();
		if (Window->SkipItems)
		{
			return NewLabel;
		}

		ImGuiContext& Context = *ImGui::GetCurrentContext();
		ImVec2 Pos = Window->DC.CursorPos;

		std::string MenuLabel(Label);
		NewLabel.reserve(MenuLabel.size());
		size_t Index = 0u;
		for (auto Char : MenuLabel)
		{
			if (Char == '^')
			{
				if (Index > 0u)
				{
					ImVec2 TextSize = ImGui::CalcTextSize(NewLabel.c_str(), NewLabel.c_str() + Index - 1u, true);
					ImVec2 CharSize = ImGui::CalcTextSize(Label + Index - 1u, Label + Index, true);

					const ImVec4& LineColor = ImGui::IsItemActivated() ? Context.Style.Colors[ImGuiCol_Text] : Context.Style.Colors[ImGuiCol_TextDisabled];
					float LineY = TextSize.y + Pos.y;
					Window->DrawList->AddLine(ImVec2(Pos.x + TextSize.x, LineY), ImVec2(Pos.x + TextSize.x + CharSize.x * 0.6f, LineY), ImGui::GetColorU32(LineColor), 1.0f);
				}
			}
			else
			{
				NewLabel.append(1u, Char);
				++Index;
			}
		}
		NewLabel.append(1u, '\0');

		return NewLabel;
	}

	bool BeginMenuWithStress(const char* Label, bool Enabled)
	{
		auto NewLabel = AddLabelStressline(Label);
		return ImGui::BeginMenu(NewLabel.empty() ? Label : NewLabel.c_str(), Enabled);
	}

	bool MenuItemWithStress(const char* Label, const char* Shortcut, bool Selected, bool Enabled)
	{
		auto NewLabel = AddLabelStressline(Label);
		return ImGui::MenuItem(NewLabel.empty() ? Label : NewLabel.c_str(), Shortcut, Selected, Enabled);
	}
}