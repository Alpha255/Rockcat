#include "Core/Math/Color.h"
#include "Core/Math/Math.h"

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

const float Color::Scaler = 1.0f / 255.0f;

Color::Color(uint32_t RGBA)
	: Vector4(
			static_cast<float>((RGBA >> 0) * 0xFF) * Scaler,
			static_cast<float>((RGBA >> 8) * 0xFF) * Scaler,
			static_cast<float>((RGBA >> 16) * 0xFF) * Scaler,
			static_cast<float>((RGBA >> 24) * 0xFF) * Scaler)
{
}

Color::Color(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
	: Vector4(R * Scaler, G * Scaler, B * Scaler, A * Scaler)
{
}

Color Color::Random()
{
	static std::default_random_engine RandomEngine((uint32_t)std::time(nullptr));
	static std::uniform_real_distribution<float> Distribution(0.0f, 1.0f);

	return Color(Distribution(RandomEngine), Distribution(RandomEngine), Distribution(RandomEngine), 1.0f);
	/*
		std::random_device rd;
		return std::bind(
			std::uniform_real_distribution<>{from, to},
			std::default_random_engine{ rd() })();
	*/
}

Color Color::ToSRGB() const
{
	DirectX::XMVECTOR T = DirectX::XMVectorSaturate(DirectX::XMLoadFloat4(this));
	DirectX::XMVECTOR RetV = DirectX::XMVectorSubtract(DirectX::XMVectorScale(DirectX::XMVectorPow(T, DirectX::XMVectorReplicate(1.0f / 2.4f)), 1.055f), DirectX::XMVectorReplicate(0.055f));
	RetV = DirectX::XMVectorSelect(RetV, DirectX::XMVectorScale(T, 12.92f), DirectX::XMVectorLess(T, DirectX::XMVectorReplicate(0.0031308f)));

	Color Ret;
	DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorSelect(T, RetV, DirectX::g_XMSelect1110));

	return Ret;
}

Color Color::FromSRGB() const
{
	DirectX::XMVECTOR T = DirectX::XMVectorSaturate(DirectX::XMLoadFloat4(this));
	DirectX::XMVECTOR RetV = DirectX::XMVectorPow(DirectX::XMVectorScale(DirectX::XMVectorAdd(T, DirectX::XMVectorReplicate(0.055f)), 1.0f / 1.055f), DirectX::XMVectorReplicate(2.4f));
	RetV = DirectX::XMVectorSelect(RetV, DirectX::XMVectorScale(T, 1.0f / 12.92f), DirectX::XMVectorLess(T, DirectX::XMVectorReplicate(0.0031308f)));

	Color Ret;
	DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorSelect(T, RetV, DirectX::g_XMSelect1110));

	return Ret;
}

Color Color::ToREC709() const
{
	DirectX::XMVECTOR T = DirectX::XMVectorSaturate(DirectX::XMLoadFloat4(this));
	DirectX::XMVECTOR RetV = DirectX::XMVectorSubtract(DirectX::XMVectorScale(DirectX::XMVectorPow(T, DirectX::XMVectorReplicate(0.45f)), 1.099f), DirectX::XMVectorReplicate(0.099f));
	RetV = DirectX::XMVectorSelect(RetV, DirectX::XMVectorScale(T, 4.5f), DirectX::XMVectorLess(T, DirectX::XMVectorReplicate(0.0018f)));

	Color Ret;
	DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorSelect(T, RetV, DirectX::g_XMSelect1110));
	return Ret;
}

Color Color::FromREC709() const
{
	DirectX::XMVECTOR T = DirectX::XMVectorSaturate(DirectX::XMLoadFloat4(this));
	DirectX::XMVECTOR RetV = DirectX::XMVectorPow(DirectX::XMVectorScale(DirectX::XMVectorAdd(T, DirectX::XMVectorReplicate(0.099f)), 1.0f / 1.099f), DirectX::XMVectorReplicate(1.0f / 0.45f));
	RetV = DirectX::XMVectorSelect(RetV, DirectX::XMVectorScale(T, 1.0f / 4.5f), DirectX::XMVectorLess(T, DirectX::XMVectorReplicate(0.0081f)));

	Color Ret;
	DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorSelect(T, RetV, DirectX::g_XMSelect1110));
	return Ret;
}

uint32_t Color::RGB10A2() const
{
	DirectX::XMVECTOR Ret = DirectX::XMVectorRound(DirectX::XMVectorMultiply(DirectX::XMVectorSaturate(DirectX::XMLoadFloat4(this)), DirectX::XMVectorSet(1023.0f, 1023.0f, 1023.0f, 3.0f)));
	Ret = _mm_castsi128_ps(_mm_cvttps_epi32(Ret));
	uint32_t R = DirectX::XMVectorGetIntX(Ret);
	uint32_t G = DirectX::XMVectorGetIntY(Ret);
	uint32_t B = DirectX::XMVectorGetIntZ(Ret);
	uint32_t A = DirectX::XMVectorGetIntW(Ret) >> 8;
	return A << 30 | B << 20 | G << 10 | R;
}

uint32_t Color::RGBA8() const
{
	DirectX::XMVECTOR Ret = DirectX::XMVectorRound(DirectX::XMVectorMultiply(DirectX::XMVectorSaturate(DirectX::XMLoadFloat4(this)), DirectX::XMVectorReplicate(255.0f)));
	Ret = _mm_castsi128_ps(_mm_cvttps_epi32(Ret));
	uint32_t R = DirectX::XMVectorGetIntX(Ret);
	uint32_t G = DirectX::XMVectorGetIntY(Ret);
	uint32_t B = DirectX::XMVectorGetIntZ(Ret);
	uint32_t A = DirectX::XMVectorGetIntW(Ret);
	return A << 24 | B << 16 | G << 8 | R;
}

uint32_t Color::RG11B10F(bool RoundToEven) const
{
#if !USE_SSE
	static const float MaxVal = float(1 << 16);
	static const float F32toF16 = (1.0 / (1ull << 56)) * (1.0 / (1ull << 56));

	union { float f; uint32_t u; } R, G, B;

	R.f = Clamp(this->R(), 0.0f, MaxVal) * F32toF16;
	G.f = Clamp(this->G(), 0.0f, MaxVal) * F32toF16;
	B.f = Clamp(this->B(), 0.0f, MaxVal) * F32toF16;

	if (RoundToEven)
	{
		// Bankers rounding:  2.5 -> 2.0  ;  3.5 -> 4.0
		R.u += 0x0FFFF + ((R.u >> 16) & 1);
		G.u += 0x0FFFF + ((G.u >> 16) & 1);
		B.u += 0x1FFFF + ((B.u >> 17) & 1);
	}
	else
	{
		// Default rounding:  2.5 -> 3.0  ;  3.5 -> 4.0
		R.u += 0x00010000;
		G.u += 0x00010000;
		B.u += 0x00020000;
	}

	R.u &= 0x0FFE0000;
	G.u &= 0x0FFE0000;
	B.u &= 0x0FFC0000;

	return R.u >> 17 | G.u >> 6 | B.u << 4;

#else // SSE
	static DirectX::XMVECTORU32 Scale = { 0x07800000, 0x07800000, 0x07800000, 0 }; // 2^-112
	static DirectX::XMVECTORU32 Round1 = { 0x00010000, 0x00010000, 0x00020000, 0 };
	static DirectX::XMVECTORU32 Round2 = { 0x0000FFFF, 0x0000FFFF, 0x0001FFFF, 0 };
	static DirectX::XMVECTORU32 Mask = { 0x0FFE0000, 0x0FFE0000, 0x0FFC0000, 0 };

	// Treat the values like integers as we clamp to [0, +Inf].  This translates 32-bit specials
	// to 16-bit specials (while also turning anything greater than MAX_HALF into +INF).
	__m128i Ti = _mm_max_epi32(_mm_castps_si128(DirectX::XMLoadFloat4(this)), _mm_setzero_si128());
	Ti = _mm_min_epi32(Ti, _mm_set1_epi32(0x47800000)); // 2^16 = 65536.0f = INF

	// Bias the exponent by -112 (-127 + 15) to denormalize values < 2^-14
	Ti = _mm_castps_si128(_mm_mul_ps(_mm_castsi128_ps(Ti), Scale));

	if (RoundToEven)
	{
		// Add 0x10000 when odd, 0x0FFFF when even (before truncating bits)
		Ti = _mm_add_epi32(Ti, _mm_max_epi32(_mm_and_si128(_mm_srli_epi32(Ti, 1), Round1), Round2));
	}
	else //if (RoundToNearest)
	{
		Ti = _mm_add_epi32(Ti, Round1);
	}

	DirectX::XMVECTORU32 Ret;
	Ret.v = _mm_castsi128_ps(_mm_and_si128(Ti, Mask));
	return Ret.u[0] >> 17 | Ret.u[1] >> 6 | Ret.u[2] << 4;
#endif
}

uint32_t Color::RGB9E5() const
{
#if !USE_SSE
	static const float MaxVal = float(0x1FF << 7);
	static const float MinVal = float(1.f / (1 << 16));

	// Clamp RGB to [0, 1.FF*2^16]
	float RR = Clamp(this->R(), 0.0f, MaxVal);
	float GG = Clamp(this->G(), 0.0f, MaxVal);
	float BB = Clamp(this->B(), 0.0f, MaxVal);

	// Compute the maximum channel, no less than 1.0*2^-15
	float MaxChannel = std::max(std::max(RR, GG), std::max(BB, MinVal));

	// Take the exponent of the maximum channel (rounding up the 9th bit) and
	// add 15 to it.  When added to the channels, it causes the implicit '1.0'
	// bit and the first 8 mantissa bits to be shifted down to the low 9 bits
	// of the mantissa, rounding the truncated bits.
	union { float f; int32_t i; } R, G, B, E;
	E.f = MaxChannel;
	E.i += 0x07804000; // Add 15 to the exponent and 0x4000 to the mantissa
	E.i &= 0x7F800000; // Zero the mantissa

	// This shifts the 9-bit values we need into the lowest bits, rounding as
	// needed.  Note that if the channel has a smaller exponent than the max
	// channel, it will shift even more.  This is intentional.
	R.f = RR + E.f;
	G.f = GG + E.f;
	B.f = BB + E.f;

	// Convert the Bias to the correct exponent in the upper 5 bits.
	E.i <<= 4;
	E.i += 0x10000000;

	// Combine the fields.  RGB floats have unwanted data in the upper 9
	// bits.  Only red needs to mask them off because green and blue shift
	// it out to the left.
	return E.i | B.i << 18 | G.i << 9 | R.i & 511;

#else // SSE
	// Clamp RGB to [0, 1.FF*2^16]
	__m128 MaxVal = _mm_castsi128_ps(_mm_set1_epi32(0x477F8000));
	__m128 RGB = _mm_min_ps(_mm_max_ps(DirectX::XMLoadFloat4(this), _mm_setzero_ps()), MaxVal);

	// Compute the maximum channel, no less than 1.0*2^-15
	__m128 MinVal = _mm_castsi128_ps(_mm_set1_epi32(0x37800000));
	__m128 MaxChannel = _mm_max_ps(RGB, MinVal);
	MaxChannel = _mm_max_ps(_mm_permute_ps(MaxChannel, _MM_SHUFFLE(3, 1, 0, 2)),
		_mm_max_ps(_mm_permute_ps(MaxChannel, _MM_SHUFFLE(3, 0, 2, 1)), MaxChannel));

	// Add 15 to the exponent and 0x4000 to the mantissa
	__m128i Bias15 = _mm_set1_epi32(0x07804000);
	__m128i ExpMask = _mm_set1_epi32(0x7F800000);
	__m128i Bias = _mm_and_si128(_mm_add_epi32(_mm_castps_si128(MaxChannel), Bias15), ExpMask);

	// rgb += Bias
	RGB = _mm_add_ps(RGB, _mm_castsi128_ps(Bias));

	// Exp = (Bias << 4) + 0x10000000;
	__m128i Exp = _mm_add_epi32(_mm_slli_epi32(Bias, 4), _mm_set1_epi32(0x10000000));

	// Combine words
	DirectX::XMVECTORU32 Ret;
	Ret.v = _mm_insert_ps(RGB, _mm_castsi128_ps(Exp), 0x30);
	return Ret.u[3] | Ret.u[2] << 18 | Ret.u[1] << 9 | Ret.u[0] & 511;
#endif
}

NAMESPACE_END(Math)
