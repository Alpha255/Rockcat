#pragma once

#include "Core/Math/Vector4.h"

NAMESPACE_START(Math)

class Color : public Vector4
{
public:
	using Vector4::Vector4;

	template<uint32_t>
	Color(uint32_t RGBA)
		: Vector4(
			(RGBA >> 0) * 0xFF * Scaler,
			(RGBA >> 8) * 0xFF * Scaler,
			(RGBA >> 16) * 0xFF * Scaler,
			(RGBA >> 24) * 0xFF * Scaler)
	{
	}

	template<uint8_t, uint8_t, uint8_t, uint8_t>
	Color(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
		: Vector4(R * Scaler, G * Scaler, B * Scaler, A * Scaler)
	{
	}

	float R() const { return x; }
	float G() const { return y; }
	float B() const { return z; }
	float A() const { return w; }

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
	static const Color Gray;
	static const Color Purple;

	static Color Random();

	Color ToSRGB() const;
	Color FromSRGB() const;
	Color ToREC709() const;
	Color FromREC709() const;

	uint32_t RGB10A2() const;
	uint32_t RGBA8() const;

	uint32_t RG11B10F(bool RoundToEven = false) const;
	uint32_t RGB9E5() const;
protected:
private:
	static const float Scaler;
};

NAMESPACE_END(Math)
