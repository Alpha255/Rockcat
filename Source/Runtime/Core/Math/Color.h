#pragma once

#include "Runtime/Core/Math/Vector4.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

class Color : public Vector4
{
public:
	using Vector4::Vector4;

	Color(const class Color32& Color);

	float32_t R() const { return x; }
	float32_t G() const { return y; }
	float32_t B() const { return z; }
	float32_t A() const { return w; }

	static const Color White;
	static const Color Black;
	static const Color Red;
	static const Color Green;
	static const Color Blue;
	static const Color Yellow;
	static const Color Cyan;
	static const Color Magenta;
	static const Color Silver;
	static const Color LightSteelBlue;
	static const Color DarkBlue;

	static Color Random();

	static Color SRGBToRGB(const Color& SRGB)
	{
		assert(0);
		return SRGB;
	}
protected:
private:
	static const float32_t Scaler;
};

class Color32
{
public:
	Color32(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255u)
		: Value{R, G, B, A}
	{
	}

	Color32(const Color& Color);

	float32_t R() const { return Value.R; }
	float32_t G() const { return Value.G; }
	float32_t B() const { return Value.B; }
	float32_t A() const { return Value.A; }

	static const Color32 White;
	static const Color32 Black;
	static const Color32 Red;
	static const Color32 Green;
	static const Color32 Blue;
	static const Color32 Yellow;
	static const Color32 Cyan;
	static const Color32 Magenta;
	static const Color32 Silver;
	static const Color32 LightSteelBlue;
	static const Color32 DarkBlue;

	static Color32 Random();
protected:
private:
	union
	{
		struct CValue
		{
			uint8_t R;
			uint8_t G;
			uint8_t B;
			uint8_t A;
		}Value;
		uint32_t UIntValue;
	};
};

NAMESPACE_END(Math)
NAMESPACE_END(Gear)
