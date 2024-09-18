#pragma once

#include "Editor/ImGuiWidget.h"
#include "Editor/ImGuiConfigurations.h"

class ImGuiPanel : public ImGuiWidget
{
public:
	enum class EStatus : uint8_t
	{
		Opened = 1 << 0,
		Closed = 1 << 1,
		Docked = 1 << 2,
		Minimized = 1 << 3,
		Maximized = 1 << 4,
		Collapsed = 1 << 5,
		Locked = 1 << 6
	};

	ImGuiPanel(const char* Title, const ImGuiWidget* Parent = nullptr, int32_t WindowsFlags = 0);

	bool IsOpened() const { return EnumHasAnyFlags(m_Status, EStatus::Opened); }
	bool IsClosed() const { return EnumHasAnyFlags(m_Status, EStatus::Closed); }
	bool IsDocked() const { return EnumHasAnyFlags(m_Status, EStatus::Docked); }
	bool IsMinimized() const { return EnumHasAnyFlags(m_Status, EStatus::Minimized); }
	bool IsMaximized() const { return EnumHasAnyFlags(m_Status, EStatus::Maximized); }
	bool IsCollapsed() const { return EnumHasAnyFlags(m_Status, EStatus::Collapsed); }
	bool IsLocked() const { return EnumHasAnyFlags(m_Status, EStatus::Locked); }

	virtual void OnOpen() {}
	virtual void OnClose() {}
	virtual void OnCollapse() {}
	virtual void OnMinimize() {}
	virtual void OnMaximize() {}

	void Draw() override;

	const char* const GetTitle() const { return m_Title.data(); }
protected:
	virtual void OnDraw() {}
private:
	EStatus m_Status = EStatus::Opened;
	bool m_Show = true;
	int32_t m_WindowFlags = 0;
	std::string_view m_Title;
};

ENUM_FLAG_OPERATORS(ImGuiPanel::EStatus)