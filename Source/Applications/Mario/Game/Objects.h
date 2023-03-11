#pragma once

#include "Object2D.h"

class Brick : public Object2D
{
public:
	Brick(uint32_t left, uint32_t top)
		: Base(eCoin, 16U, 16U, left, top)
	{
		m_State.TexCount = 4U;
	}
	~Brick() = default;

	virtual void Update(float) override;
protected:
private:
};

class Coin : public Object2D
{
public:
	Coin(uint32_t left, uint32_t top)
		: Base(
			eCoin,
			16U,
			32U,
			left * eWidth + 8, 
			top * eHeight - 20)
	{
		m_State.TexCount = 3U;

		m_Acceleration.y = (float)eGravity;
		m_Velocity.y = -500.0f;
	}
	~Coin() = default;

	virtual void Update(float) override;
protected:
private:
};

class Bullet : public Object2D
{
public:
	Bullet(uint32_t left, uint32_t top)
		: Base(eBullet, 16U, 16U, left, top)
	{
	}
	~Bullet() = default;

	virtual void Update(float) override;
protected:
private:
};

class Exploder : public Object2D
{
public:
	Exploder(uint32_t left, uint32_t top)
		: Base(eExplode, 32U, 32U, left, top)
	{
		m_State.TexCount = 3U;
	}
	~Exploder() = default;

	virtual void Update(float) override;
protected:
private:
};

class Mushroom : public Object2D
{
public:
	Mushroom(uint32_t left, uint32_t top)
		: Base(eMushroom, 32U, 32U, left, top)
	{
		m_Acceleration.y = (float)eGravity;
		m_Velocity = Vec2(85.0f, -100.0f);
	}
	~Mushroom() = default;

	virtual void Update(float) override;
protected:
private:
};

class Monster : public Object2D
{
public:
	Monster(uint32_t left, uint32_t top)
		: Base(eMonster, 32U, 32U, left, top)
	{
		m_State.TexCount = 6U;

		m_Velocity.x = -75.0f;
	};
	~Monster() = default;

	virtual void Update(float) override {}
protected:
private:
};

class Flower : public Object2D
{
public:
	Flower(uint32_t left, uint32_t top)
		: Base(
			eFlower, 
			32U,
			32U,
			left * eWidth, 
			top * eHeight - 33)
	{
	}
protected:
private:
};

class Turtle : public Object2D
{
public:
	Turtle(eType type, uint32_t left, uint32_t top)
		: Base(type, 32U, 48U, left, top)
	{
		m_State.TexCount = 6U;

		if (eWalkingTurtle == type)
		{
			m_Velocity.x = -80.0f;
		}
		else if (eFlyingTurtle == type)
		{
			m_Velocity.x = -50.0f;
			m_Acceleration.y = (float)eGravity / 2.0f;
		}
		else
		{
			assert(0);
		}
	}
	~Turtle() = default;

	virtual void Update(float) override {}
protected:
private:
};
