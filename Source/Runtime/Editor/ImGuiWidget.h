#pragma once

#include "Core/Definitions.h"
#include "Core/Name.h"

class ImGuiWidget
{
public:
	ImGuiWidget(FName&& Name, const ImGuiWidget* Parent = nullptr)
		: m_Name(std::move(Name))
		, m_Parent(Parent)
	{
	}

	virtual ~ImGuiWidget() = default;

	const FName& GetName() const { return m_Name; }
	void SetName(FName&& Name) { m_Name = std::move(Name); }

	virtual void Draw() = 0;
private:
	const ImGuiWidget* m_Parent;
	FName m_Name;
};
