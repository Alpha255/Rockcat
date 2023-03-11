#include "Core/Math/Color.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

const Color Color::White(1.0f, 1.0f, 1.0f, 1.0f);
const Color Color::Black(0.0f, 0.0f, 0.0f, 1.0f);
const Color Color::Red(1.0f, 0.0f, 0.0f, 1.0f);
const Color Color::Green(0.0f, 1.0f, 0.0f, 1.0f);
const Color Color::Blue(0.0f, 0.0f, 1.0f, 1.0f);
const Color Color::Yellow(1.0f, 1.0f, 0.0f, 1.0f);
const Color Color::Cyan(0.0f, 1.0f, 1.0f, 1.0f);
const Color Color::Magenta(1.0f, 0.0f, 1.0f, 1.0f);
const Color Color::Silver(0.75f, 0.75f, 0.75f, 1.0f);
const Color Color::LightSteelBlue(0.69f, 0.77f, 0.87f, 1.0f);
const Color Color::DarkBlue(0.0f, 0.125f, 0.3f, 1.0f);

const float32_t Color::Scaler = 1.0f / 255.0f;

Color::Color(const Color32& Color)
	: Vector4(Color.R() * Scaler, Color.G() * Scaler, Color.B() * Scaler, Color.A() * Scaler)
{
}

Color Color::Random()
{
	static std::default_random_engine RandomEngine((uint32_t)std::time(nullptr));
	static std::uniform_real_distribution<float32_t> Distribution(0.0f, 1.0f);

	return Color(Distribution(RandomEngine), Distribution(RandomEngine), Distribution(RandomEngine), 1.0f);
	/*
		std::random_device rd;
		return std::bind(
			std::uniform_real_distribution<>{from, to},
			std::default_random_engine{ rd() })();
	*/
}

const Color32 Color32::White(255u, 255u, 255u);
const Color32 Color32::Black(0u, 0u, 0u);
const Color32 Color32::Red(255u, 0u, 0u);
const Color32 Color32::Green(0u, 255u, 0u);
const Color32 Color32::Blue(0u, 0u, 255u);
const Color32 Color32::Yellow(255u, 255u, 0u);
const Color32 Color32::Cyan(0u, 255u, 255u);
const Color32 Color32::Magenta(255u, 0u, 255u);
const Color32 Color32::Silver(189u, 195u, 199u);
const Color32 Color32::LightSteelBlue(175u, 196u, 221u);
const Color32 Color32::DarkBlue(0u, 31u, 76u);

Color32::Color32(const Color& Color)
	: Color32(
		static_cast<uint8_t>(Color.x * 255u), 
		static_cast<uint8_t>(Color.y * 255u), 
		static_cast<uint8_t>(Color.z * 255u), 
		static_cast<uint8_t>(Color.w * 255u))
{
}

Color32 Color32::Random()
{
	return Color32(Color::Random());
}

NAMESPACE_END(Math)
NAMESPACE_END(Gear)
