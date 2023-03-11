#pragma once

#include "D3DMath.h"

class Object2D
{
public:
	enum eType
	{
		eBrickDebris,
		eBullet,
		eCoin,
		eExplode,
		eFlower,
		eMario,
		eMushroom,
		eTile,
		eMonster,
		eWalkingTurtle,
		eFlyingTurtle,
		eTypeCount
	};

	enum eOrientation
	{
		eLeft,
		eRight
	};

	enum eMotion
	{
		eNone,
		eStatic,
		eWalk,
		eCrouch,
		eJump,
		eStop,
		eSlide,
		eDead,
		eUpsidedown
	};

	enum eTileAttribute
	{
		eWidth = 32,
		eHeight = 32,
		eGravity = 1200
	};

	struct Area
	{
		uint32_t Left = 0U;
		uint32_t Top = 0U;   /// Coordinate in window

		uint32_t ObjectWidth = 0U;
		uint32_t ObjectHeight = 0U;

		uint32_t UVX = 0U;
		uint32_t UVY = 0U;
	};

	struct State
	{
		eOrientation Orientation = eLeft;
		eMotion Motion = eNone;

		uint32_t TexCount = 1U;
		uint32_t TexIndex = 0U;
	};

	Object2D() = default;
	virtual ~Object2D() = default;

	Object2D(eType type, uint32_t width, uint32_t height, uint32_t left = 0U, uint32_t top = 0U);

	inline const class Image *GetImage() const
	{
		assert(m_Image);
		return m_Image;
	}

	inline const Area &GetArea() const
	{
		return m_Area;
	}

	inline const State &GetState() const
	{
		return m_State;
	}

	inline void UpdateState(eOrientation orientation, eMotion motion)
	{
		m_State.Orientation = orientation;
		m_State.Motion = motion;
	}

	inline void UpdateArea(uint32_t top, uint32_t left, uint32_t width, uint32_t height, uint32_t uvX, uint32_t uvY)
	{
		m_Area.Top = top;
		m_Area.Left = left;
		m_Area.ObjectWidth = width;
		m_Area.ObjectHeight = height;
		m_Area.UVX = uvX;
		m_Area.UVY = uvY;
	}

	inline uint32_t ObjectWidth() const
	{
		assert(m_ObjectWidth > 0U);
		return m_ObjectWidth;
	}

	inline uint32_t ObjectHeight() const
	{
		assert(m_ObjectHeight > 0U);
		return m_ObjectHeight;
	}

	void virtual Update(float /*elapseTime*/) {}

	bool IsCollide(const Object2D &object);
	bool IsCollide(const class Map &map);
protected:
	eType m_Type = eTypeCount;

	Area m_Area;
	State m_State;

	uint32_t m_ObjectWidth = 0U;
	uint32_t m_ObjectHeight = 0U;

	Vec2 m_Velocity = {};
	Vec2 m_Acceleration = {};

	const class Image *m_Image = nullptr;
	typedef Object2D Base;
private:
};