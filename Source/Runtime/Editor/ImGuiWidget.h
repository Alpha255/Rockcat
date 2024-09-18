#pragma once

#include "Core/Definitions.h"

class ImGuiWidget
{
public:
	ImGuiWidget(const char* const Name = nullptr, const ImGuiWidget* Parent = nullptr)
		: m_Name(Name)
		, m_Parent(Parent)
	{
	}

	virtual ~ImGuiWidget() = default;

	const char* const GetName() const { return m_Name.data(); }
	void SetName(const char* const Name) { m_Name = Name; }

	virtual void Draw() = 0;
private:
	const ImGuiWidget* m_Parent;
	std::string_view m_Name;
};
