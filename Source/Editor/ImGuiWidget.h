#pragma once

#include "Core/Definitions.h"
#include "Core/Name.h"

class ImGuiWidget
{
public:
	ImGuiWidget(Name&& Name, const ImGuiWidget* Parent = nullptr)
		: m_Name(std::move(Name))
		, m_Parent(Parent)
	{
	}

	virtual ~ImGuiWidget() = default;

	const Name& GetName() const { return m_Name; }
	void SetName(Name&& Name) { m_Name = std::move(Name); }

	virtual void Draw() = 0;
private:
	const ImGuiWidget* m_Parent;
	Name m_Name;
};
