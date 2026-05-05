#pragma once

NAMESPACE_START(Math)

template<class T>
struct Point
{
	Point()
		: X(0), Y(0)
	{
	}

	Point(T InX, T InY)
		: X(InX), Y(InY)
	{
	}

	Point(T InValue)
		: X(InValue), Y(InValue)
	{
	}

	Point(const Point&) = default;
	Point& operator=(const Point&) = default;

	T X, Y;
};

template<class T>
class Rect
{
public:
	Rect()
		: m_LeftTop(0, 0), m_RightBottom(0, 0)
	{
	}

	Rect(T InLeft, T InTop, T InRight, T InBottom)
		: m_LeftTop(InLeft, InTop), m_RightBottom(InRight, InBottom)
	{
	}

	Rect(const Rect& Other)
		: m_LeftTop(Other.m_LeftTop), m_RightBottom(Other.m_RightBottom)
	{
	}

	Rect& operator=(const Rect&) = default;

	inline T Width() const { return m_RightBottom.X - m_LeftTop.X; }
	inline T Height() const { return m_RightBottom.Y - m_LeftTop.Y; }
	inline T Area() const { return Width() * Height(); }

	bool IsIntersect(const Rect& Other) const
	{
		return !(m_LeftTop.X >= Other.m_RightBottom.X || m_RightBottom.X <= Other.m_LeftTop.X || m_LeftTop.Y >= Other.m_RightBottom.Y || m_RightBottom.Y <= Other.m_LeftTop.Y);
	}

	bool Contains(const Point<T>& Point) const
	{
		return Point.X >= m_LeftTop.X && Point.X <= m_RightBottom.X && Point.Y >= m_LeftTop.Y && Point.Y <= m_RightBottom.Y;
	}

	void Scale(T ScaleX, T ScaleY)
	{
		m_LeftTop.X *= ScaleX;
		m_LeftTop.Y *= ScaleY;
		m_RightBottom.X *= ScaleX;
		m_RightBottom.Y *= ScaleY;
	}

	void Union(const Rect& Other)
	{
		m_LeftTop.X = std::min(m_LeftTop.X, Other.m_LeftTop.X);
		m_LeftTop.Y = std::min(m_LeftTop.Y, Other.m_LeftTop.Y);
		m_RightBottom.X = std::max(m_RightBottom.X, Other.m_RightBottom.X);
		m_RightBottom.Y = std::max(m_RightBottom.Y, Other.m_RightBottom.Y);
	}
protected:
private:
	Point<T> m_LeftTop;
	Point<T> m_RightBottom;
};

NAMESPACE_END(Math)
