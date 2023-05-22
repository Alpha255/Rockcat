#pragma once

#include "Runtime/Core/Definitions.h"

class ITickable
{
public:
	enum ETickType : uint8_t
	{
		Conditional,
		Always,
		Never
	};

	virtual ~ITickable() = default;

	virtual void Tick(float32_t ElapsedSeconds) = 0;

	virtual bool8_t IsTickable() const { return true; }

	ETickType GetTickType() const { return m_TickType; }

	void SetTickType(ETickType TickType) { m_TickType = TickType; }

	static void TickObjects(float32_t /*ElapsedSeconds*/) {}
protected:
	ETickType m_TickType = ETickType::Always;
private:
};

