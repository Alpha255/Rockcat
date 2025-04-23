#pragma once

#include "Engine/RHI/RHIResource.h"
#include "Engine/Asset/Asset.h"

enum class ERHITextureDimension : uint8_t
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

struct RHISubresource
{
	static constexpr uint32_t AllMipLevels = ~0u;
	static constexpr uint32_t AllArrayLayers = ~0u;

	uint32_t BaseMipLevel = 0u;
	uint32_t NumMips = 1u;

	uint32_t BaseArrayLayer = 0u;
	uint32_t NumLayers = 1u;

	struct Hasher
	{
		size_t operator()(const RHISubresource& Subresource) const noexcept
		{
			return ComputeHash(Subresource.BaseMipLevel, Subresource.NumMips, Subresource.BaseArrayLayer, Subresource.NumLayers);
		}
	};

	bool operator==(const RHISubresource& Other) const
	{
		return BaseMipLevel == Other.BaseMipLevel &&
			NumMips == Other.NumMips &&
			BaseArrayLayer == Other.BaseArrayLayer &&
			NumLayers == Other.NumLayers;
	}

	bool operator!=(const RHISubresource& Other) const
	{
		return BaseMipLevel != Other.BaseMipLevel ||
			NumMips != Other.NumMips ||
			BaseArrayLayer != Other.BaseArrayLayer ||
			NumLayers != Other.NumLayers;
	}
};

namespace RHI
{
	const static RHISubresource AllSubresource{0u, RHISubresource::AllMipLevels, 0u, RHISubresource::AllArrayLayers};
}

struct RHITextureCreateInfo
{
	uint32_t Width = 1u;
	uint32_t Height = 1u;
	uint32_t Depth = 1u;
	uint16_t NumArrayLayer = 1u;
	uint16_t NumMipLevel = 1u;

	ERHITextureDimension Dimension = ERHITextureDimension::Unknown;
	ERHISampleCount SampleCount = ERHISampleCount::Sample_1_Bit;

	ERHIFormat Format = ERHIFormat::Unknown;
	ERHIBufferUsageFlags BufferUsageFlags = ERHIBufferUsageFlags::None;

	ERHIResourceState PermanentState = ERHIResourceState::Unknown;

	DataBlock InitialData;

	std::string Name;

	inline RHITextureCreateInfo& SetWidth(uint32_t Value) { Width = Value; return *this; }
	inline RHITextureCreateInfo& SetHeight(uint32_t Value) { Height = Value; return *this; }
	inline RHITextureCreateInfo& SetDepth(uint32_t Value) { Depth = Value; return *this; }
	inline RHITextureCreateInfo& SetNumMipLevel(uint32_t Value) { NumMipLevel = static_cast<uint16_t>(Value); return *this; }
	inline RHITextureCreateInfo& SetNumArrayLayer(uint32_t Value) { NumArrayLayer = static_cast<uint16_t>(Value); return *this; }
	inline RHITextureCreateInfo& SetFormat(ERHIFormat Value) { Format = Value; return *this; }
	inline RHITextureCreateInfo& SetDimension(ERHITextureDimension Value) { Dimension = Value; return *this; }
	inline RHITextureCreateInfo& SetSampleCount(ERHISampleCount Count) { SampleCount = Count; return *this; }
	inline RHITextureCreateInfo& SetUsages(ERHIBufferUsageFlags UsageFlags) { BufferUsageFlags = BufferUsageFlags | UsageFlags; return *this; };
	inline RHITextureCreateInfo& SetPermanentState(ERHIResourceState States) { PermanentState = States; return *this; }
	inline RHITextureCreateInfo& SetInitialData(const DataBlock& Data) { InitialData = Data; return *this; }
	inline RHITextureCreateInfo& SetInitialData(size_t Size, const std::shared_ptr<std::byte>& Data) { InitialData.Size = Size; InitialData.Data = Data; return *this; }

	template<class T>
	inline RHITextureCreateInfo& SetName(T&& InName) { Name = std::move(std::string(std::forward<T>(InName))); return *this; }
};

class RHITexture : public RHIResource
{
public:
	RHITexture(const RHITextureCreateInfo& CreateInfo)
		: m_Width(CreateInfo.Width)
		, m_Height(CreateInfo.Height)
		, m_Depth(CreateInfo.Depth)
		, m_NumArrayLayer(CreateInfo.NumArrayLayer)
		, m_NumMipLevel(CreateInfo.NumMipLevel)
		, m_Dimension(CreateInfo.Dimension)
		, m_Format(CreateInfo.Format)
		, m_State(CreateInfo.PermanentState)
		, RHIResource(CreateInfo.Name.c_str())
	{
	}

	inline uint32_t GetWidth() const { return m_Width; }
	inline uint32_t GetHeight() const { return m_Height; }
	inline uint32_t GetDepth() const { return m_Depth; }
	inline uint32_t GetNumArrayLayer() const { return m_NumArrayLayer; }
	inline uint32_t GetNumMipLevel() const { return m_NumMipLevel; }
	inline ERHITextureDimension GetDimension() const { return m_Dimension; }
	inline ERHIFormat GetFormat() const { return m_Format; }
	inline ERHIResourceState GetState() const { return m_State; }
private:
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_Depth;

	uint32_t m_NumArrayLayer;
	uint32_t m_NumMipLevel;

	ERHITextureDimension m_Dimension;
	ERHIFormat m_Format;
	ERHIResourceState m_State;
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

	float MaxAnisotropy = 0.0f;
	float MipLODBias = 0.0f;
	float MinLOD = 0.0f;
	float MaxLOD = 0.0f;

	std::string Name;
	
	inline RHISamplerCreateInfo& SetMinMagFilter(ERHIFilter Filter) { MinMagFilter = Filter; return *this; }
	inline RHISamplerCreateInfo& SetMipmapMode(ERHIFilter Mode) { MipmapMode = Mode; return *this; }
	inline RHISamplerCreateInfo& SetSamplerReduction(ESamplerReduction SamplerReduction) { Reduction = SamplerReduction; return *this; }
	inline RHISamplerCreateInfo& SetAddressModeU(ERHISamplerAddressMode SamplerAddressMode) { AddressModeU = SamplerAddressMode; return *this; };
	inline RHISamplerCreateInfo& SetAddressModeV(ERHISamplerAddressMode SamplerAddressMode) { AddressModeV = SamplerAddressMode; return *this; };
	inline RHISamplerCreateInfo& SetAddressModeW(ERHISamplerAddressMode SamplerAddressMode) { AddressModeW = SamplerAddressMode; return *this; };
	inline RHISamplerCreateInfo& SetCompareOp(ERHICompareFunc CompareFunc) { CompareOp = CompareFunc; return *this; }
	inline RHISamplerCreateInfo& SetBorderColor(ERHIBorderColor BorderColorMode) { BorderColor = BorderColorMode; return *this; }
	inline RHISamplerCreateInfo& SetMaxAnisotropy(float Anisotropy) { MaxAnisotropy = Anisotropy; return *this; }
	inline RHISamplerCreateInfo& SetMipLODBias(float MipLODBiasValue) { MipLODBias = MipLODBiasValue; return *this; }
	inline RHISamplerCreateInfo& SetMinLOD(float MinLODValue) { MinLOD = MinLODValue; return *this; }
	inline RHISamplerCreateInfo& SetMaxLOD(float MaxLODValue) { MaxLOD = MaxLODValue; return *this; }

	template<class T>
	inline RHISamplerCreateInfo& SetName(T&& InName) { Name = std::move(std::string(std::forward<T>(InName))); return *this; }
};

class RHISampler : public RHIResource
{
public:
	RHISampler(const RHISamplerCreateInfo& RHICreateInfo)
		: RHIResource(RHICreateInfo.Name.c_str())
	{
	}
};
