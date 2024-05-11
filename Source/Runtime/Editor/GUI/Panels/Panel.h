#pragma once

#include "Core/Math/Vector2.h"

class Panel
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

	Panel(const char* Title, const char* Icon = nullptr);
	virtual ~Panel() = default;

	bool IsOpened() const { return EnumHasAnyFlags(m_Status, EStatus::Opened); }
	bool IsClosed() const { return EnumHasAnyFlags(m_Status, EStatus::Closed); }
	bool IsDocked() const { return EnumHasAnyFlags(m_Status, EStatus::Docked); }
	bool IsMinimized() const { return EnumHasAnyFlags(m_Status, EStatus::Minimized); }
	bool IsMaximized() const { return EnumHasAnyFlags(m_Status, EStatus::Maximized); }
	bool IsCollapsed() const { return EnumHasAnyFlags(m_Status, EStatus::Collapsed); }
	bool IsLocked() const { return EnumHasAnyFlags(m_Status, EStatus::Locked); }

	virtual void Open() {};
	virtual void Close() {};

	virtual void OnGUI() = 0;

	void Draw();

	void Collapse();
	void Minimize();
	void Maximize();
protected:
	virtual bool Begin();
	virtual void End();
	bool ShouldDraw() const;
private:
	EStatus m_Status = EStatus::Opened;
	bool m_Show = true;
	int32_t m_WindowFlags = 0;
	Math::Vector2 m_Pos;
	Math::Vector2 m_Size;
	std::string_view m_Title;
};

ENUM_FLAG_OPERATORS(Panel::EStatus)