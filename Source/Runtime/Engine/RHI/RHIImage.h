#pragma once

#include "Runtime/Engine/RHI/RHIRenderStates.h"

//#define STBI_NO_GIF
//#define STBI_NO_PIC
//#define STBI_NO_PNM

enum class ERHIImageType : uint32_t
{
	Unknown,
	T_1D,
	T_1D_Array,
	T_2D,
	T_2D_Array,
	T_3D,
	T_Cube,
	T_Cube_Array,
	Buffer
};

struct RHIImageSubresourceRange
{
	static constexpr uint32_t AllMipLevels = ~0u;
	static constexpr uint32_t AllArrayLayers = ~0u;

	uint32_t BaseMipLevel = 0u;
	uint32_t LevelCount = 1u;

	uint32_t BaseArrayLayer = 0u;
	uint32_t LayerCount = 1u;

	struct Hasher
	{
		size_t operator()(const RHIImageSubresourceRange& SubresourceRange) const noexcept
		{
			size_t Hash = 0u;
			HashCombine(Hash, 
				SubresourceRange.BaseMipLevel,
				SubresourceRange.LevelCount,
				SubresourceRange.BaseArrayLayer,
				SubresourceRange.LayerCount);
			return Hash;
		}
	};

	bool8_t operator==(const RHIImageSubresourceRange& Other) const
	{
		return BaseMipLevel == Other.BaseMipLevel &&
			LevelCount == Other.LevelCount &&
			BaseArrayLayer == Other.BaseArrayLayer &&
			LayerCount == Other.LayerCount;
	}

	bool8_t operator!=(const RHIImageSubresourceRange& Other) const
	{
		return BaseMipLevel != Other.BaseMipLevel ||
			LevelCount != Other.LevelCount ||
			BaseArrayLayer != Other.BaseArrayLayer ||
			LayerCount != Other.LayerCount;
	}
};

namespace RHI
{
	const static RHIImageSubresourceRange AllImageSubresource{0u, RHIImageSubresourceRange::AllMipLevels, 0u, RHIImageSubresourceRange::AllArrayLayers};
}

struct RHIImageCreateInfo
{
	uint32_t Width = 1u;
	uint32_t Height = 1u;
	uint32_t Depth = 1u;

	uint16_t ArrayLayers = 1u;
	uint16_t MipLevels = 1u;

	ERHIImageType ImageType = ERHIImageType::Unknown;
	ERHIFormat Format = ERHIFormat::Unknown;

	ERHISampleCount SampleCount = ERHISampleCount::Sample_1_Bit;
	ERHIBufferUsageFlags BufferUsageFlags = ERHIBufferUsageFlags::None;

	ERHIResourceState RequiredState = ERHIResourceState::Common;

	std::string Name;

	inline RHIImageCreateInfo& SetWidth(uint32_t Value) { Width = Value; return *this; }
	inline RHIImageCreateInfo& SetHeight(uint32_t Value) { Height = Value; return *this; }
	inline RHIImageCreateInfo& SetDepth(uint32_t Value) { Depth = Value; return *this; }
	inline RHIImageCreateInfo& SetMipLevels(uint32_t Value) { MipLevels = static_cast<uint16_t>(Value); return *this; }
	inline RHIImageCreateInfo& SetArrayLayers(uint32_t Value) { ArrayLayers = static_cast<uint16_t>(Value); return *this; }
	inline RHIImageCreateInfo& SetFormat(ERHIFormat Value) { Format = Value; return *this; }
	inline RHIImageCreateInfo& SetImageType(ERHIImageType Value) { ImageType = Value; return *this; }
	inline RHIImageCreateInfo& SetSampleCount(ERHISampleCount Count) { SampleCount = Count; return *this; }
	inline RHIImageCreateInfo& SetUsages(ERHIBufferUsageFlags UsageFlags) { BufferUsageFlags = BufferUsageFlags | UsageFlags; return *this; };
	inline RHIImageCreateInfo& SetRequiredState(ERHIResourceState State) { RequiredState = State; return *this; }
	inline RHIImageCreateInfo& SetName(const char* Value) { Name = Value; return *this; }
	inline RHIImageCreateInfo& SetName(const std::string& Value) { Name = Value; return *this; }
};

class RHIImage
{
public:
	RHIImage(const RHIImageCreateInfo& CreateInfo)
		: m_Width(CreateInfo.Width)
		, m_Height(CreateInfo.Height)
		, m_Depth(CreateInfo.Depth)
		, m_ArrayLayers(CreateInfo.ArrayLayers)
		, m_MipLevels(CreateInfo.MipLevels)
		, m_Type(CreateInfo.ImageType)
		, m_Format(CreateInfo.Format)
	{
	}

	inline uint32_t GetWidth() const { return m_Width; }
	inline uint32_t GetHeight() const { return m_Height; }
	inline uint32_t GetDepth() const { return m_Depth; }
	inline uint32_t GetArrayLayers() const { return m_ArrayLayers; }
	inline uint32_t GetMipLevels() const { return m_MipLevels; }
	inline ERHIImageType GetImageType() const { return m_Type; }
	inline ERHIFormat GetFormat() const { return m_Format; }
private:
	uint32_t m_Width = 1u;
	uint32_t m_Height = 1u;
	uint32_t m_Depth = 1u;

	uint32_t m_ArrayLayers = 1u;
	uint32_t m_MipLevels = 1u;

	ERHIImageType m_Type = ERHIImageType::Unknown;
	ERHIFormat m_Format = ERHIFormat::Unknown;
};

enum class ERHIFilter : uint8_t
{
	Nearest,
	Linear  /// Cubic ???
};

enum class ERHISamplerAddressMode : uint8_t
{
	Repeat,
	MirroredRepeat,
	ClampToEdge,
	ClampToBorder,
	MirrorClampToEdge
};

enum class ERHIBorderColor : uint8_t
{
	FloatTransparentBlack,
	IntTransparentBlack,
	FloatOpaqueBlack,
	IntOpaqueBlack,
	FloatOpaqueWhite,
	IntOpaqueWhite
};

enum class ESamplerReduction : uint8_t
{
	Standard,
	Comparison,
	Minimum,
	Maximum
};

struct RHISamplerCreateInfo
{
	ERHIFilter MinMagFilter = ERHIFilter::Nearest;
	ERHIFilter MipmapMode = ERHIFilter::Nearest;
	ESamplerReduction Reduction = ESamplerReduction::Standard;
	ERHISamplerAddressMode AddressModeU = ERHISamplerAddressMode::Repeat;
	ERHISamplerAddressMode AddressModeV = ERHISamplerAddressMode::Repeat;
	ERHISamplerAddressMode AddressModeW = ERHISamplerAddressMode::Repeat;
	ERHICompareFunc CompareOp = ERHICompareFunc::Less;
	ERHIBorderColor BorderColor = ERHIBorderColor::FloatTransparentBlack;

	float32_t MaxAnisotropy = 0.0f;
	float32_t MipLODBias = 0.0f;
	float32_t MinLOD = 0.0f;
	float32_t MaxLOD = 0.0f;
	
	inline RHISamplerCreateInfo& SetMinMagFilter(ERHIFilter Filter) { MinMagFilter = Filter; return *this; }
	inline RHISamplerCreateInfo& SetMipmapMode(ERHIFilter Mode) { MipmapMode = Mode; return *this; }
	inline RHISamplerCreateInfo& SetSamplerReduction(ESamplerReduction SamplerReduction) { Reduction = SamplerReduction; return *this; }
	inline RHISamplerCreateInfo& SetAddressModeU(ERHISamplerAddressMode SamplerAddressMode) { AddressModeU = SamplerAddressMode; return *this; };
	inline RHISamplerCreateInfo& SetAddressModeV(ERHISamplerAddressMode SamplerAddressMode) { AddressModeV = SamplerAddressMode; return *this; };
	inline RHISamplerCreateInfo& SetAddressModeW(ERHISamplerAddressMode SamplerAddressMode) { AddressModeW = SamplerAddressMode; return *this; };
	inline RHISamplerCreateInfo& SetCompareOp(ERHICompareFunc CompareFunc) { CompareOp = CompareFunc; return *this; }
	inline RHISamplerCreateInfo& SetBorderColor(ERHIBorderColor BorderColorMode) { BorderColor = BorderColorMode; return *this; }
	inline RHISamplerCreateInfo& SetMaxAnisotropy(float32_t Anisotropy) { MaxAnisotropy = Anisotropy; return *this; }
	inline RHISamplerCreateInfo& SetMipLODBias(float32_t MipLODBiasValue) { MipLODBias = MipLODBiasValue; return *this; }
	inline RHISamplerCreateInfo& SetMinLOD(float32_t MinLODValue) { MinLOD = MinLODValue; return *this; }
	inline RHISamplerCreateInfo& SetMaxLOD(float32_t MaxLODValue) { MaxLOD = MaxLODValue; return *this; }
};

class RHISampler
{
};
