#pragma once

#include "Colorful/IRenderer/IRenderStates.h"
#include "Runtime/Asset/Asset.h"

NAMESPACE_START(RHI)

#define STBI_NO_GIF
#define STBI_NO_PIC
#define STBI_NO_PNM

enum class EImageType : uint32_t
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

struct ImageSubresourceRange
{
	static constexpr uint32_t AllMipLevels = std::numeric_limits<uint32_t>().max();
	static constexpr uint32_t AllArrayLayers = std::numeric_limits<uint32_t>().max();

	uint32_t BaseMipLevel = 0u;
	uint32_t LevelCount = 1u;

	uint32_t BaseArrayLayer = 0u;
	uint32_t LayerCount = 1u;

	struct Hasher
	{
		size_t operator()(const ImageSubresourceRange& SubresourceRange) const noexcept
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

	bool8_t operator==(const ImageSubresourceRange& Other) const
	{
		return BaseMipLevel == Other.BaseMipLevel &&
			LevelCount == Other.LevelCount &&
			BaseArrayLayer == Other.BaseArrayLayer &&
			LayerCount == Other.LayerCount;
	}

	bool8_t operator!=(const ImageSubresourceRange& Other) const
	{
		return BaseMipLevel != Other.BaseMipLevel ||
			LevelCount != Other.LevelCount ||
			BaseArrayLayer != Other.BaseArrayLayer ||
			LayerCount != Other.LayerCount;
	}
};

const static ImageSubresourceRange AllSubresource{0u, ImageSubresourceRange::AllMipLevels, 0u, ImageSubresourceRange::AllArrayLayers};

struct ImageSubresourceDesc
{
	uint32_t Width = 1u;
	uint32_t Height = 1u;
	uint32_t Depth = 1u;

	uint32_t MipLevel = 0u;
	uint32_t ArrayLayer = 0u;

	uint32_t Offset = 0u;
	uint32_t RowBytes = 0u;
	uint32_t SliceBytes = 0u;
};

struct ImageSlice
{
	int32_t X = 0u;
	int32_t Y = 0u;
	int32_t Z = 0u;

	uint32_t Width = 1u;
	uint32_t Height = 1u;
	uint32_t Depth = 1u;

	uint32_t MipLevel = 1u;
	uint32_t ArrayLayer = 1u;
};

struct ImageDesc
{
	uint32_t Width = 1u;
	uint32_t Height = 1u;
	uint32_t Depth = 1u;

	uint32_t ArrayLayers = 1u;
	uint32_t MipLevels = 1u;

	EImageType Type = EImageType::Unknown;
	EFormat Format = EFormat::Unknown;

	ESampleCount SampleCount = ESampleCount::Sample_1_Bit;
	EBufferUsageFlags Usages = EBufferUsageFlags::None;

	EResourceState RequiredState = EResourceState::Common;

	std::string Name;

	std::vector<ImageSubresourceDesc> Subresources;

	struct AssetData
	{
		const byte8_t* Data = nullptr;
		uint32_t Size = 0u;
	};
	AssetData Asset;

	inline ImageDesc& SetWidth(uint32_t Value) { Width = Value; return *this; }
	inline ImageDesc& SetHeight(uint32_t Value) { Height = Value; return *this; }
	inline ImageDesc& SetDepth(uint32_t Value) { Depth = Value; return *this; }
	inline ImageDesc& SetMipLevels(uint32_t Value) { MipLevels = Value; return *this; }
	inline ImageDesc& SetArrayLayers(uint32_t Value) { ArrayLayers = Value; return *this; }
	inline ImageDesc& SetFormat(EFormat Value) { Format = Value; return *this; }
	inline ImageDesc& SetImageType(EImageType Value) { Type = Value; return *this; }
	inline ImageDesc& SetSampleCount(ESampleCount Count) { SampleCount = Count; return *this; }
	inline ImageDesc& SetUsages(EBufferUsageFlags UsageFlags) { Usages = Usages | UsageFlags; return *this; };
	inline ImageDesc& SetRequiredState(EResourceState State) { RequiredState = State; return *this; }
	inline ImageDesc& SetName(const char8_t* DebugName) { Name = DebugName; return *this; }
	inline ImageDesc& SetData(const byte8_t* Data) { Asset.Data = Data; return *this; }
	inline ImageDesc& SetDataSize(uint32_t Size) { Asset.Size = Size; return *this; }
};

class IImage : public IHWResource
{
public:
	IImage(const ImageDesc& Desc)
		: IHWResource(Desc.Name.c_str())
		, m_Width(Desc.Width)
		, m_Height(Desc.Height)
		, m_Depth(Desc.Depth)
		, m_ArrayLayers(Desc.ArrayLayers)
		, m_MipLevels(Desc.MipLevels)
		, m_Type(Desc.Type)
		, m_Format(Desc.Format)
	{
	}

	uint32_t Width() const { return m_Width; }
	uint32_t Height() const { return m_Height; }
	uint32_t Depth() const { return m_Depth; }
	uint32_t ArrayLayers() const { return m_ArrayLayers; }
	uint32_t MipLevels() const { return m_MipLevels; }
	EImageType Type() const { return m_Type; }
	EFormat Format() const { return m_Format; }
protected:
private:
	uint32_t m_Width = 1u;
	uint32_t m_Height = 1u;
	uint32_t m_Depth = 1u;

	uint32_t m_ArrayLayers = 1u;
	uint32_t m_MipLevels = 1u;

	EImageType m_Type = EImageType::Unknown;
	EFormat m_Format = EFormat::Unknown;
};

enum class EFilter : uint8_t
{
	Nearest,
	Linear  /// Cubic ???
};

enum class ESamplerAddressMode : uint8_t
{
	Repeat,
	MirroredRepeat,
	ClampToEdge,
	ClampToBorder,
	MirrorClampToEdge
};

enum class EBorderColor : uint8_t
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

struct SamplerDesc : public IHashedResourceDesc
{
	EFilter MinMagFilter = EFilter::Nearest;
	EFilter MipmapMode = EFilter::Nearest;
	ESamplerReduction Reduction = ESamplerReduction::Standard;
	ESamplerAddressMode AddressModeU = ESamplerAddressMode::Repeat;
	ESamplerAddressMode AddressModeV = ESamplerAddressMode::Repeat;
	ESamplerAddressMode AddressModeW = ESamplerAddressMode::Repeat;
	ECompareFunc CompareOp = ECompareFunc::Less;
	EBorderColor BorderColor = EBorderColor::FloatTransparentBlack;

	uint32_t MaxAnisotropy = 0u;
	float32_t MipLODBias = 0.0f;
	float32_t MinLOD = 0.0f;
	float32_t MaxLOD = 0.0f;

	size_t Hash() const override final
	{
		if (HashValue == InvalidHash)
		{
			HashValue = ComputeHash(
				MinMagFilter,
				MipmapMode,
				Reduction,
				AddressModeU,
				AddressModeV,
				AddressModeW,
				CompareOp,
				BorderColor,
				MaxAnisotropy,
				MipLODBias,
				MinLOD,
				MaxLOD);
		}

		return HashValue;
	}
	
	inline SamplerDesc& SetMinMagFilter(EFilter Filter) { MinMagFilter = Filter; return *this; }
	inline SamplerDesc& SetMipmapMode(EFilter Mode) { MipmapMode = Mode; return *this; }
	inline SamplerDesc& SetSamplerReduction(ESamplerReduction SamplerReduction) { Reduction = SamplerReduction; return *this; }
	inline SamplerDesc& SetAddressModeU(ESamplerAddressMode SamplerAddressMode) { AddressModeU = SamplerAddressMode; return *this; };
	inline SamplerDesc& SetAddressModeV(ESamplerAddressMode SamplerAddressMode) { AddressModeV = SamplerAddressMode; return *this; };
	inline SamplerDesc& SetAddressModeW(ESamplerAddressMode SamplerAddressMode) { AddressModeW = SamplerAddressMode; return *this; };
	inline SamplerDesc& SetCompareOp(ECompareFunc CompareFunc) { CompareOp = CompareFunc; return *this; }
	inline SamplerDesc& SetBorderColor(EBorderColor BorderColorMode) { BorderColor = BorderColorMode; return *this; }
	inline SamplerDesc& SetMaxAnisotropy(uint32_t Anisotropy) { MaxAnisotropy = Anisotropy; return *this; }
	inline SamplerDesc& SetMipLODBias(float32_t MipLODBiasValue) { MipLODBias = MipLODBiasValue; return *this; }
	inline SamplerDesc& SetMinLOD(float32_t MinLODValue) { MinLOD = MinLODValue; return *this; }
	inline SamplerDesc& SetMaxLOD(float32_t MaxLODValue) { MaxLOD = MaxLODValue; return *this; }
};

class ISampler : public IHWResource
{
};

NAMESPACE_END(RHI)

using ImageAsset = Asset<RHI::IImage, IAsset::ECategory::Image>;
