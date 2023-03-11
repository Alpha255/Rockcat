#pragma once

#include "Colorful/IRenderer/IResource.h"

NAMESPACE_START(RHI)

enum class EPolygonMode : uint8_t
{
	Solid,
	Wireframe
};

enum class ECullMode : uint8_t
{
	None,
	FrontFace,
	BackFace
};

enum class EFrontFace : uint8_t
{
	Counterclockwise,
	Clockwise
};

enum class EBlendFactor : uint8_t
{
	Zero,
	One,
	Constant,
	InverseConstant,

	SrcAlpha,
	InverseSrcAlpha,
	DstAlpha,
	InverseDstAlpha,
	SrcAlphaSaturate,
	Src1Alpha,
	InverseSrc1Alpha,

	SrcColor,
	InverseSrcColor,
	DstColor,
	InverseDstColor,
	Src1Color,
	InverseSrc1Color
};

enum class EBlendOp : uint8_t
{
	Add,
	Subtract,
	ReverseSubtract,
	Min,
	Max
};

enum class EColorWriteMask : uint8_t
{
	None,
	Red = 1 << 0,
	Green = 1 << 1,
	Blue = 1 << 2,
	Alpha = 1 << 3,
	All = Red | Green | Blue | Alpha
};
ENUM_FLAG_OPERATORS(EColorWriteMask)

enum class ELogicOp : uint8_t
{
	Clear,
	And,
	And_Reverse,
	Copy,
	And_Inverted,
	No,
	Xor,
	Or,
	Nor,
	Equivalent,
	Invert,
	Or_Reverse,
	Copy_Inverted,
	Or_Inverted,
	Nand,
	Set
};

enum class ECompareFunc : uint8_t
{
	Always,
	Never,
	Less,
	LessOrEqual,
	Equal,
	NotEqual,
	Greater,
	GreaterOrEqual
};

enum class EStencilOp : uint8_t
{
	Keep,
	Zero,
	Replace,                 /// Set the stencil data to the reference value
	IncrementAndClamp,       /// Increment the stencil value by 1, and clamp the result
	DecrementAndClamp,       /// Decrement the stencil value by 1, and clamp the result
	Invert,
	IncrementAndWrap,        /// Increment the stencil value by 1, and wrap the result if necessary
	DecrementAndWrap         /// Increment the stencil value by 1, and wrap the result if necessary
};

enum class ESampleCount : uint8_t
{
	Sample_1_Bit = 1 << 0,
	Sample_2_Bit = 1 << 1,
	Sample_4_Bit = 1 << 2,
	Sample_8_Bit = 1 << 3,
	Sample_16_Bit = 1 << 4,
	Sample_32_Bit = 1 << 5,
	Sample_64_Bit = 1 << 6
};

enum class EPrimitiveTopology : uint8_t
{
	PointList,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip,
	LineListAdjacency,
	LineStripAdjacency,
	TriangleListAdjacency,
	TriangleStripAdjacency,
	PatchList
};

struct RenderTargetBlendDesc
{
	uint8_t Index = ELimitations::MaxRenderTargets;

	bool8_t Enable = false;
	EColorWriteMask ColorMask = EColorWriteMask::All;

	EBlendFactor SrcColor = EBlendFactor::One;
	EBlendFactor DstColor = EBlendFactor::Zero;
	EBlendOp ColorOp = EBlendOp::Add;

	EBlendFactor SrcAlpha = EBlendFactor::One;
	EBlendFactor DstAlpha = EBlendFactor::Zero;
	EBlendOp AlphaOp = EBlendOp::Add;

	inline RenderTargetBlendDesc& SetIndex(uint8_t Value) { Index = Value; return *this; }
	inline RenderTargetBlendDesc& SetEnable(bool8_t IsEnable) { Enable = IsEnable; return *this; }
	inline RenderTargetBlendDesc& SetColorMask(EColorWriteMask ColorWriteMask) { ColorMask = ColorWriteMask; return *this; }
	inline RenderTargetBlendDesc& SetSrcColorBlendFactor(EBlendFactor SrcColorBlendFactor) { SrcColor = SrcColorBlendFactor; return *this; }
	inline RenderTargetBlendDesc& SetDstColorBlendFactor(EBlendFactor DstColorBlendFactor) { DstColor = DstColorBlendFactor; return *this; }
	inline RenderTargetBlendDesc& SetColorBlendOp(EBlendOp ColorBlendOp) { ColorOp = ColorBlendOp; return *this; }
	inline RenderTargetBlendDesc& SetSrcAlphaBlendFactor(EBlendFactor SrcAlphaBlendFactor) { SrcAlpha = SrcAlphaBlendFactor; return *this; }
	inline RenderTargetBlendDesc& SetDstAlphaBlendFactor(EBlendFactor DstAlphaBlendFactor) { DstAlpha = DstAlphaBlendFactor; return *this; }
	inline RenderTargetBlendDesc& SetAlphaBlendOp(EBlendOp AlphaBlendOp) { AlphaOp = AlphaBlendOp; return *this; }
};

/// Move AlphaToCoverage/IndependentBlend to MultisampleState
struct BlendStateDesc : public IHashedResourceDesc
{
	bool8_t EnableLogicOp = false;
	ELogicOp LogicOp = ELogicOp::No;

	RenderTargetBlendDesc RenderTargetBlends[ELimitations::MaxRenderTargets];

	inline BlendStateDesc& SetEnableLogicOp(bool8_t Enable) { EnableLogicOp = Enable; return *this; }
	inline BlendStateDesc& SetLogicOp(ELogicOp Op) { LogicOp = Op; return *this; }

	size_t Hash() const override final
	{
		if (HashValue == InvalidHash)
		{
			HashValue = ComputeHash(EnableLogicOp, LogicOp);
			for (uint32_t Index = 0u; Index < ELimitations::MaxRenderTargets; ++Index)
			{
				if (RenderTargetBlends[Index].Index != ELimitations::MaxRenderTargets)
				{
					HashCombine(HashValue, ComputeHash(
						RenderTargetBlends[Index].Index,
						RenderTargetBlends[Index].Enable,
						RenderTargetBlends[Index].ColorMask,
						RenderTargetBlends[Index].SrcColor,
						RenderTargetBlends[Index].DstColor,
						RenderTargetBlends[Index].ColorOp,
						RenderTargetBlends[Index].SrcAlpha,
						RenderTargetBlends[Index].DstAlpha,
						RenderTargetBlends[Index].AlphaOp));
				}
			}
		}

		return HashValue;
	}
};

struct RasterizationStateDesc : public IHashedResourceDesc
{
	EPolygonMode PolygonMode = EPolygonMode::Solid;
	ECullMode CullMode = ECullMode::BackFace;
	EFrontFace FrontFace = EFrontFace::Clockwise;
	bool8_t EnableDepthClamp = false;

	float32_t DepthBias = 0.0f;       /// A scalar factor controlling the constant depth value added to each fragment.
	float32_t DepthBiasClamp = 0.0f;  /// The maximum (or minimum) depth bias of a fragment.
	float32_t DepthBiasSlope = 0.0f;  /// A scalar factor applied to a fragment¡¯s slope in depth bias calculations.
	float32_t LineWidth = 1.0f;

	inline RasterizationStateDesc& SetPolygonMode(EPolygonMode Mode) { PolygonMode = Mode; return *this; }
	inline RasterizationStateDesc& SetCullMode(ECullMode Mode) { CullMode = Mode; return *this; }
	inline RasterizationStateDesc& SetFrontFace(EFrontFace Face) { FrontFace = Face; return *this; }
	inline RasterizationStateDesc& SetEnableDepthClamp(bool8_t Enable) { EnableDepthClamp = Enable; return *this; }
	inline RasterizationStateDesc& SetDepthBias(float32_t Bias) { DepthBias = Bias; return *this; }
	inline RasterizationStateDesc& SetDepthBiasClamp(float32_t BiasClamp) { DepthBiasClamp = BiasClamp; return *this; }
	inline RasterizationStateDesc& SetDepthBiasSlope(float32_t BiasSlope) { DepthBiasSlope = BiasSlope; return *this; }
	inline RasterizationStateDesc& SetLineWidth(float32_t Width) { LineWidth = Width; return *this; }

	size_t Hash() const override final
	{
		if (HashValue == InvalidHash)
		{
			HashValue = ComputeHash(
				PolygonMode,
				CullMode,
				FrontFace,
				EnableDepthClamp,
				DepthBias,
				DepthBiasClamp,
				DepthBiasSlope,
				LineWidth);
		}

		return HashValue;
	}
};

struct StencilStateDesc
{
	EStencilOp FailOp = EStencilOp::Keep;
	EStencilOp PassOp = EStencilOp::Keep;
	EStencilOp DepthFailOp = EStencilOp::Keep;
	ECompareFunc CompareFunc = ECompareFunc::Always;

	uint32_t Ref = 0u;
};

struct DepthStencilStateDesc : public IHashedResourceDesc
{
	bool8_t EnableDepth = true;
	bool8_t EnableDepthWrite = false;
	bool8_t EnableDepthBoundsTest = false;
	bool8_t EnableStencil = false;

	ECompareFunc DepthCompareFunc = ECompareFunc::Less;
	uint8_t StencilReadMask = 0xFF;
	uint8_t StencilWriteMask = 0xFF;

	Vector2 DepthBounds;

	StencilStateDesc FrontFaceStencil;
	StencilStateDesc BackFaceStencil;

	inline DepthStencilStateDesc& SetEnableDepth(bool8_t Enable) { EnableDepth = Enable; return *this; }
	inline DepthStencilStateDesc& SetEnableDepthWrite(bool8_t Enable) { EnableDepthWrite = Enable; return *this; }
	inline DepthStencilStateDesc& SetEnableDepthBoundsTest(bool8_t Enable) { EnableDepthBoundsTest = Enable; return *this; }
	inline DepthStencilStateDesc& SetEnableStencil(bool8_t Enable) { EnableStencil = Enable; return *this; }
	inline DepthStencilStateDesc& SetDepthCompareFunc(ECompareFunc CompareFunc) { DepthCompareFunc = CompareFunc; return *this; }
	inline DepthStencilStateDesc& SetStencilReadMask(uint8_t Mask) { StencilReadMask = Mask; return *this; }
	inline DepthStencilStateDesc& SetStencilWriteMask(uint8_t Mask) { StencilWriteMask = Mask; return *this; }
	inline DepthStencilStateDesc& SetDepthBounds(const Vector2& Bounds) { DepthBounds = Bounds; return *this; }
	inline DepthStencilStateDesc& SetStencilFailOpFrontFace(EStencilOp Op) { FrontFaceStencil.FailOp = Op; return *this; }
	inline DepthStencilStateDesc& SetStencilPassOpFrontFace(EStencilOp Op) { FrontFaceStencil.PassOp = Op; return *this; }
	inline DepthStencilStateDesc& SetDepthFailOpFrontFace(EStencilOp Op) { FrontFaceStencil.DepthFailOp = Op; return *this; }
	inline DepthStencilStateDesc& SetCompareFuncFrontFace(ECompareFunc Func) { FrontFaceStencil.CompareFunc = Func; return *this; }
	inline DepthStencilStateDesc& SetRefFrontFace(uint32_t Ref) { FrontFaceStencil.Ref = Ref; return *this; }
	inline DepthStencilStateDesc& SetStencilFailOpBackFace(EStencilOp Op) { BackFaceStencil.FailOp = Op; return *this; }
	inline DepthStencilStateDesc& SetStencilPassOpBackFace(EStencilOp Op) { BackFaceStencil.PassOp = Op; return *this; }
	inline DepthStencilStateDesc& SetDepthFailOpBackFace(EStencilOp Op) { BackFaceStencil.DepthFailOp = Op; return *this; }
	inline DepthStencilStateDesc& SetCompareFuncBackFace(ECompareFunc Func) { BackFaceStencil.CompareFunc = Func; return *this; }
	inline DepthStencilStateDesc& SetRefBackFace(uint32_t Ref) { BackFaceStencil.Ref = Ref; return *this; }

	size_t Hash() const override final
	{
		if (HashValue == InvalidHash)
		{
			HashValue = ComputeHash(
				EnableDepth,
				EnableDepthWrite,
				EnableDepthBoundsTest,
				EnableStencil,
				DepthCompareFunc,
				StencilReadMask,
				StencilWriteMask);

			HashCombine(HashValue, ComputeHash(
				FrontFaceStencil.FailOp,
				FrontFaceStencil.PassOp,
				FrontFaceStencil.DepthFailOp,
				FrontFaceStencil.CompareFunc,
				FrontFaceStencil.Ref));

			HashCombine(HashValue, ComputeHash(
				BackFaceStencil.FailOp,
				BackFaceStencil.PassOp,
				BackFaceStencil.DepthFailOp,
				BackFaceStencil.CompareFunc,
				BackFaceStencil.Ref));
		}

		return HashValue;
	}
};

struct MultisampleStateDesc : public IHashedResourceDesc
{
	ESampleCount SampleCount = ESampleCount::Sample_1_Bit;
	bool8_t EnableSampleShading = false;
	bool8_t EnableAlphaToCoverage = false;
	bool8_t EnableAlphaToOne = false;

	float32_t MinSampleShading = 0.0f;
	uint32_t* SampleMask = nullptr; /// ??? 

	size_t Hash() const override final
	{
		if (HashValue == InvalidHash)
		{
			HashValue = ComputeHash(
				SampleCount,
				EnableSampleShading,
				EnableAlphaToCoverage,
				EnableAlphaToOne,
				MinSampleShading,
				SampleMask);
		}

		return HashValue;
	}
};

struct Viewport
{
	Vector2 LeftTop;
	Vector2 Extent;
	Vector2 Z;

	Viewport()
		: LeftTop(0.0f, 0.0f)
		, Extent(0.0f, 0.0f)
		, Z(0.0f, 1.0f)
	{
	}

	Viewport(float32_t Width, float32_t Height)
		: LeftTop(0.0f, 0.0f)
		, Extent(Width, Height)
		, Z(0.0f, 1.0f)
	{
	}

	Viewport(float32_t Left, float32_t Top, float32_t Width, float32_t Height, float32_t MinZ = 0.0f, float32_t MaxZ = 1.0f)
		: LeftTop(Left, Top)
		, Extent(Width, Height)
		, Z(MinZ, MaxZ)
	{
	}

	float32_t Width() const
	{
		return Extent.x;
	}

	float32_t Height() const
	{
		return Extent.y;
	}

	bool8_t operator==(const Viewport& Other) const
	{
		return LeftTop == Other.LeftTop && Extent == Other.Extent && Z == Other.Z;
	}

	bool8_t operator!=(const Viewport& Other) const
	{
		return LeftTop != Other.LeftTop || Extent != Other.Extent || Z != Other.Z;
	}
};

struct ScissorRect
{
	Vector2 LeftTop;
	Vector2 Extent;

	ScissorRect()
		: LeftTop(0.0f, 0.0f)
		, Extent(0.0f, 0.0f)
	{
	}

	ScissorRect(float32_t Width, float32_t Height)
		: LeftTop(0.0f, 0.0f)
		, Extent(Width, Height)
	{
		assert(Width > 0.0f && Height > 0.0f);
	}

	ScissorRect(float32_t Left, float32_t Top, float32_t Width, float32_t Height)
		: LeftTop(Left, Top)
		, Extent(Width, Height)
	{
		assert(Width > 0.0f && Height > 0.0f);
	}

	bool8_t operator==(const ScissorRect& Other) const
	{
		return LeftTop == Other.LeftTop && Extent == Other.Extent;
	}

	bool8_t operator!=(const ScissorRect& Other) const
	{
		return LeftTop != Other.LeftTop || Extent != Other.Extent;
	}
};

struct RenderPassDesc : public IHashedResourceDesc
{
	enum class EAttachmentLoadOp
	{
		Load,
		Clear,
		DontCare
	};

	enum class EAttachmentStoreOp
	{
		Store,
		DontCare
	};

	struct AttachmentReference
	{
		uint32_t Index = ~0u; /// VK_ATTACHMENT_UNUSED
		//EImageLayout Layout = Texture::EImageLayout::Undefined;
	};

	struct AttachmentDesc
	{
		EFormat Format = EFormat::Unknown;
		uint8_t SampleCount = 1u;
		EAttachmentLoadOp LoadOp = EAttachmentLoadOp::Clear;
		EAttachmentStoreOp StoreOp = EAttachmentStoreOp::DontCare;
		EAttachmentLoadOp StencilLoadOp = EAttachmentLoadOp::Clear;
		EAttachmentStoreOp StencilStoreOp = EAttachmentStoreOp::DontCare;
		//Texture::EImageLayout InitialLayout = Texture::EImageLayout::Undefined;
		//Texture::EImageLayout FinalLayout = Texture::EImageLayout::Undefined;
	};

	struct SubPassDependency
	{
		uint32_t SrcSubpassIndex = ~0u;  /// VK_SUBPASS_EXTERNAL
		uint32_t DstSubpassIndex = ~0u;
		//GfxFlags SrcStageFlags = 0u;
		//GfxFlags DstStageFlags = 0u;
		//GfxFlags SrcAccessFlags = 0u;
		//GfxFlags DstAccessFlags = 0u;
	};

	struct SubPassDesc
	{
		std::vector<AttachmentReference> InputAttachments;
		std::vector<AttachmentReference> ColorAttachments;
		std::vector<AttachmentReference> ResolveAttachments;
		std::vector<uint32_t> PreserveAttachments;
		AttachmentReference DepthStencilAttachment;
	};

	std::vector<AttachmentDesc> AttachmentDescs;
	std::vector<SubPassDesc> SubPasses;
	std::vector<SubPassDependency> SubPassDependencies;

	size_t Hash() const override final
	{
		if (HashValue == InvalidHash)
		{
			HashValue = ComputeHash(AttachmentDescs.size());
			for (auto& attachmentDesc : AttachmentDescs)
			{
				HashCombine(HashValue, attachmentDesc.Format);
				HashCombine(HashValue, attachmentDesc.SampleCount);
			}
		}

		return HashValue;
	}
};

NAMESPACE_END(RHI)
