#pragma once

#include "RHI/RHITexture.h"
#include "Core/Math/Vector4.h"

enum class ERHIPolygonMode : uint8_t
{
	Solid,
	Wireframe
};

enum class ERHICullMode : uint8_t
{
	None,
	FrontFace,
	BackFace,
	FrontAndBackFace
};

enum class ERHIFrontFace : uint8_t
{
	Counterclockwise,
	Clockwise
};

enum class ERHIBlendFactor : uint8_t
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

enum class ERHIBlendOp : uint8_t
{
	Add,
	Subtract,
	ReverseSubtract,
	Min,
	Max
};

enum class ERHIColorWriteMask : uint8_t
{
	None,
	Red = 1 << 0,
	Green = 1 << 1,
	Blue = 1 << 2,
	Alpha = 1 << 3,
	All = Red | Green | Blue | Alpha
};
ENUM_FLAG_OPERATORS(ERHIColorWriteMask)

enum class ERHILogicOp : uint8_t
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

enum class ERHIStencilOp : uint8_t
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

enum class ERHIPrimitiveTopology : uint8_t
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
	PatchList,
	Num
};

struct RHIRenderTargetBlendDesc
{
	bool Enable = false;
	ERHIColorWriteMask ColorMask = ERHIColorWriteMask::All;

	ERHIBlendFactor SrcColor = ERHIBlendFactor::One;
	ERHIBlendFactor DstColor = ERHIBlendFactor::Zero;
	ERHIBlendOp ColorOp = ERHIBlendOp::Add;

	ERHIBlendFactor SrcAlpha = ERHIBlendFactor::One;
	ERHIBlendFactor DstAlpha = ERHIBlendFactor::Zero;
	ERHIBlendOp AlphaOp = ERHIBlendOp::Add;

	inline RHIRenderTargetBlendDesc& SetEnable(bool IsEnable) { Enable = IsEnable; return *this; }
	inline RHIRenderTargetBlendDesc& SetColorMask(ERHIColorWriteMask ColorWriteMask) { ColorMask = ColorWriteMask; return *this; }
	inline RHIRenderTargetBlendDesc& SetSrcColorBlendFactor(ERHIBlendFactor SrcColorBlendFactor) { SrcColor = SrcColorBlendFactor; return *this; }
	inline RHIRenderTargetBlendDesc& SetDstColorBlendFactor(ERHIBlendFactor DstColorBlendFactor) { DstColor = DstColorBlendFactor; return *this; }
	inline RHIRenderTargetBlendDesc& SetColorBlendOp(ERHIBlendOp ColorBlendOp) { ColorOp = ColorBlendOp; return *this; }
	inline RHIRenderTargetBlendDesc& SetSrcAlphaBlendFactor(ERHIBlendFactor SrcAlphaBlendFactor) { SrcAlpha = SrcAlphaBlendFactor; return *this; }
	inline RHIRenderTargetBlendDesc& SetDstAlphaBlendFactor(ERHIBlendFactor DstAlphaBlendFactor) { DstAlpha = DstAlphaBlendFactor; return *this; }
	inline RHIRenderTargetBlendDesc& SetAlphaBlendOp(ERHIBlendOp AlphaBlendOp) { AlphaOp = AlphaBlendOp; return *this; }
};

/// Move AlphaToCoverage/IndependentBlend to MultisampleState
struct RHIBlendStateCreateInfo
{
	bool EnableLogicOp = false;
	ERHILogicOp LogicOp = ERHILogicOp::No;

	RHIRenderTargetBlendDesc RenderTargetBlends[ERHILimitations::MaxRenderTargets];

	inline RHIBlendStateCreateInfo& SetEnableLogicOp(bool Enable) { EnableLogicOp = Enable; return *this; }
	inline RHIBlendStateCreateInfo& SetLogicOp(ERHILogicOp Op) { LogicOp = Op; return *this; }
};

struct RHIRasterizationStateCreateInfo
{
	ERHIPolygonMode PolygonMode = ERHIPolygonMode::Solid;
	ERHICullMode CullMode = ERHICullMode::BackFace;
	ERHIFrontFace FrontFace = ERHIFrontFace::Clockwise;
	bool EnableDepthClamp = false;

	float DepthBias = 0.0f;       /// A scalar factor controlling the constant depth value added to each fragment.
	float DepthBiasClamp = 0.0f;  /// The maximum (or minimum) depth bias of a fragment.
	float DepthBiasSlope = 0.0f;  /// A scalar factor applied to a fragment's slope in depth bias calculations.
	float LineWidth = 1.0f;

	inline RHIRasterizationStateCreateInfo& SetPolygonMode(ERHIPolygonMode Mode) { PolygonMode = Mode; return *this; }
	inline RHIRasterizationStateCreateInfo& SetCullMode(ERHICullMode Mode) { CullMode = Mode; return *this; }
	inline RHIRasterizationStateCreateInfo& SetFrontFace(ERHIFrontFace Face) { FrontFace = Face; return *this; }
	inline RHIRasterizationStateCreateInfo& SetEnableDepthClamp(bool Enable) { EnableDepthClamp = Enable; return *this; }
	inline RHIRasterizationStateCreateInfo& SetDepthBias(float Bias) { DepthBias = Bias; return *this; }
	inline RHIRasterizationStateCreateInfo& SetDepthBiasClamp(float BiasClamp) { DepthBiasClamp = BiasClamp; return *this; }
	inline RHIRasterizationStateCreateInfo& SetDepthBiasSlope(float BiasSlope) { DepthBiasSlope = BiasSlope; return *this; }
	inline RHIRasterizationStateCreateInfo& SetLineWidth(float Width) { LineWidth = Width; return *this; }
};

struct RHIStencilStateDesc
{
	ERHIStencilOp FailOp = ERHIStencilOp::Keep;
	ERHIStencilOp PassOp = ERHIStencilOp::Keep;
	ERHIStencilOp DepthFailOp = ERHIStencilOp::Keep;
	ERHICompareFunc CompareFunc = ERHICompareFunc::Always;

	uint32_t Ref = 0;
};

struct RHIDepthStencilStateCreateInfo
{
	bool EnableDepth = false;
	bool EnableDepthWrite = false;
	bool EnableDepthBoundsTest = false;
	bool EnableStencil = false;

	ERHICompareFunc DepthCompareFunc = ERHICompareFunc::Always;
	uint8_t StencilReadMask = 0xFF;
	uint8_t StencilWriteMask = 0xFF;

	Math::Vector2 DepthBounds;

	RHIStencilStateDesc FrontFaceStencil;
	RHIStencilStateDesc BackFaceStencil;

	inline RHIDepthStencilStateCreateInfo& SetEnableDepth(bool Enable) { EnableDepth = Enable; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetEnableDepthWrite(bool Enable) { EnableDepthWrite = Enable; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetEnableDepthBoundsTest(bool Enable) { EnableDepthBoundsTest = Enable; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetEnableStencil(bool Enable) { EnableStencil = Enable; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetDepthCompareFunc(ERHICompareFunc CompareFunc) { DepthCompareFunc = CompareFunc; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetStencilReadMask(uint8_t Mask) { StencilReadMask = Mask; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetStencilWriteMask(uint8_t Mask) { StencilWriteMask = Mask; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetDepthBounds(const Math::Vector2& Bounds) { DepthBounds = Bounds; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetStencilFailOpFrontFace(ERHIStencilOp Op) { FrontFaceStencil.FailOp = Op; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetStencilPassOpFrontFace(ERHIStencilOp Op) { FrontFaceStencil.PassOp = Op; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetDepthFailOpFrontFace(ERHIStencilOp Op) { FrontFaceStencil.DepthFailOp = Op; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetCompareFuncFrontFace(ERHICompareFunc Func) { FrontFaceStencil.CompareFunc = Func; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetRefFrontFace(uint32_t Ref) { FrontFaceStencil.Ref = Ref; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetStencilFailOpBackFace(ERHIStencilOp Op) { BackFaceStencil.FailOp = Op; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetStencilPassOpBackFace(ERHIStencilOp Op) { BackFaceStencil.PassOp = Op; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetDepthFailOpBackFace(ERHIStencilOp Op) { BackFaceStencil.DepthFailOp = Op; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetCompareFuncBackFace(ERHICompareFunc Func) { BackFaceStencil.CompareFunc = Func; return *this; }
	inline RHIDepthStencilStateCreateInfo& SetRefBackFace(uint32_t Ref) { BackFaceStencil.Ref = Ref; return *this; }
};

struct RHIMultisampleStateCreateInfo
{
	ERHISampleCount SampleCount = ERHISampleCount::Sample_1_Bit;
	bool EnableSampleShading = false;
	bool EnableAlphaToCoverage = false;
	bool EnableAlphaToOne = false;

	float MinSampleShading = 0.0f;
	uint32_t* SampleMask = nullptr; /// ??? 

	inline RHIMultisampleStateCreateInfo& SetSampleCount(ERHISampleCount Count) { SampleCount = Count; return *this; }
	inline RHIMultisampleStateCreateInfo& SetEnableSampleShading(bool Enable) { EnableSampleShading = Enable; return *this; }
	inline RHIMultisampleStateCreateInfo& SetEnableAlphaToCoverage(bool Enable) { EnableAlphaToCoverage = Enable; return *this; }
	inline RHIMultisampleStateCreateInfo& SetEnableAlphaToOne(bool Enable) { EnableAlphaToOne = Enable; return *this; }
	inline RHIMultisampleStateCreateInfo& SetMinSampleShading(float SampleShading) { MinSampleShading = SampleShading; return *this; }
	inline RHIMultisampleStateCreateInfo& SetSampleMask(uint32_t* Mask) { SampleMask = Mask; return *this; }
};

struct RHIViewport
{
	Math::Vector2 LeftTop;
	Math::Vector2 Extent;
	Math::Vector2 DepthRange;

	RHIViewport()
		: LeftTop(0.0f, 0.0f)
		, Extent(0.0f, 0.0f)
		, DepthRange(0.0f, 1.0f)
	{
	}

	RHIViewport(float Width, float Height)
		: LeftTop(0.0f, 0.0f)
		, Extent(Width, Height)
		, DepthRange(0.0f, 1.0f)
	{
	}

	RHIViewport(uint32_t Width, uint32_t Height)
		: LeftTop(0.0f, 0.0f)
		, Extent(static_cast<float>(Width), static_cast<float>(Height))
		, DepthRange(0.0f, 1.0f)
	{
	}

	RHIViewport(float Left, float Top, float Width, float Height, float MinZ = 0.0f, float MaxZ = 1.0f)
		: LeftTop(Left, Top)
		, Extent(Width, Height)
		, DepthRange(MinZ, MaxZ)
	{
	}

	float GetWidth() const { return Extent.x; }
	float GetHeight() const { return Extent.y; }

	bool operator==(const RHIViewport& Other) const { return LeftTop == Other.LeftTop && Extent == Other.Extent && DepthRange == Other.DepthRange; }
	bool operator!=(const RHIViewport& Other) const { return LeftTop != Other.LeftTop || Extent != Other.Extent || DepthRange != Other.DepthRange; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(LeftTop),
			CEREAL_NVP(Extent),
			CEREAL_NVP(DepthRange)
		);
	}
};

struct RHIScissorRect
{
	Math::Int2 LeftTop;
	Math::UInt2 Extent;

	RHIScissorRect()
		: LeftTop(0, 0)
		, Extent(0u, 0u)
	{
	}

	RHIScissorRect(uint32_t Width, uint32_t Height)
		: LeftTop(0, 0)
		, Extent(Width, Height)
	{
		assert(Width > 0u && Height > 0u);
	}

	RHIScissorRect(int32_t Left, int32_t Top, uint32_t Width, uint32_t Height)
		: LeftTop(Left, Top)
		, Extent(Width, Height)
	{
		assert(Width > 0.0f && Height > 0.0f);
	}

	uint32_t GetWidth() const { return Extent.x; }
	uint32_t GetHeight() const { return Extent.y; }

	inline bool operator==(const RHIScissorRect& Other) const { return LeftTop == Other.LeftTop && Extent == Other.Extent; }
	inline bool operator!=(const RHIScissorRect& Other) const { return LeftTop != Other.LeftTop || Extent != Other.Extent; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(LeftTop),
			CEREAL_NVP(Extent)
		);
	}
};

enum class ERHILoadOp : uint8_t
{
	None,
	Load,
	Clear,
	DontCare
};

enum class ERHIStoreOp : uint8_t
{
	None,
	Store,
	DontCare
};

struct RHIRenderPassCreateInfo
{
	struct RHIAttachment
	{
		ERHIFormat Format = ERHIFormat::Unknown;

		ERHILoadOp LoadOp = ERHILoadOp::None;
		ERHIStoreOp StoreOp = ERHIStoreOp::None;

		ERHILoadOp StencilLoadOp = ERHILoadOp::None;
		ERHIStoreOp StencilStoreOp = ERHIStoreOp::None;
	};

	ERHISampleCount SampleCount = ERHISampleCount::Sample_1_Bit;

	std::vector<RHIAttachment> ColorAttachments;
	RHIAttachment DepthStencilAttachment;

	inline bool HasDepthStencil() const { return RHI::IsDepthStencil(DepthStencilAttachment.Format) || RHI::IsDepth(DepthStencilAttachment.Format); }
	inline uint32_t GetNumColorAttachments() const { return NumColorAttachments; }
	inline RHIRenderPassCreateInfo& SetSampleCount(ERHISampleCount InSampleCount) { SampleCount = InSampleCount; return *this; }

	inline RHIRenderPassCreateInfo& AddColorAttachment(ERHIFormat Format, ERHILoadOp LoadOp = ERHILoadOp::DontCare, ERHIStoreOp StoreOp = ERHIStoreOp::Store)
	{
		assert(RHI::IsColor(Format) && ColorAttachments.size() < (ERHILimitations::MaxRenderTargets - 1u));

		ColorAttachments.emplace_back(RHIAttachment
			{
				Format,
				LoadOp,
				StoreOp,
				ERHILoadOp::DontCare,
				ERHIStoreOp::DontCare
			});
		++NumColorAttachments;
		return *this;
	}

	inline RHIRenderPassCreateInfo& SetColorAttachment(uint32_t Index, ERHIFormat Format, ERHILoadOp LoadOp = ERHILoadOp::DontCare, ERHIStoreOp StoreOp = ERHIStoreOp::Store)
	{
		assert(RHI::IsColor(Format) && Index < ColorAttachments.size() && Index < ERHILimitations::MaxRenderTargets);

		ColorAttachments[Index].Format = Format;
		ColorAttachments[Index].LoadOp = LoadOp;
		ColorAttachments[Index].StoreOp = StoreOp;

		return *this;
	}

	inline RHIRenderPassCreateInfo& SetDepthStencilAttachment(
		ERHIFormat Format,
		ERHILoadOp LoadOp = ERHILoadOp::DontCare,
		ERHIStoreOp StoreOp = ERHIStoreOp::Store,
		ERHILoadOp StencilLoadOp = ERHILoadOp::DontCare,
		ERHIStoreOp StencilStoreOp = ERHIStoreOp::Store)
	{
		assert(RHI::IsDepthStencil(Format) || RHI::IsDepth(Format));

		DepthStencilAttachment.Format = Format;
		DepthStencilAttachment.LoadOp = LoadOp;
		DepthStencilAttachment.StoreOp = StoreOp;
		DepthStencilAttachment.StencilLoadOp = StencilLoadOp;
		DepthStencilAttachment.StencilStoreOp = StencilStoreOp;

		return *this;
	}
private:
	uint32_t NumColorAttachments = 0u;
};

struct RHIVertexBuffer
{
	uint32_t Location = 0;
	size_t Offset = 0u;
	const RHIBuffer* Buffer = nullptr;
};

struct RHIVertexStream
{
	RHIVertexStream& Add(uint32_t Location, size_t Offset, const RHIBuffer* Buffer)
	{
		assert(Buffer && Location < ERHILimitations::MaxVertexStreams);
		VertexBuffers.emplace_back(RHIVertexBuffer{ Location, Offset, Buffer });
		return *this;
	}

	std::vector<RHIVertexBuffer> VertexBuffers;
};

struct RHISwapchainCreateInfo
{
	const void* WindowHandle = nullptr;
	uint32_t Width = 0u;
	uint32_t Height = 0u;
	bool Fullscreen = false;
	bool VSync = false;
	bool HDR = false;

	inline RHISwapchainCreateInfo& SetWindowHandle(const void* InWindowHandle) { WindowHandle = InWindowHandle; return *this; }
	inline RHISwapchainCreateInfo& SetWidth(uint32_t InWidth) { Width = InWidth; return *this; }
	inline RHISwapchainCreateInfo& SetHeight(uint32_t InHeight) { Height = InHeight; return *this; }
	inline RHISwapchainCreateInfo& SetFullscreen(bool InFullscreen) { Fullscreen = InFullscreen; return *this; }
	inline RHISwapchainCreateInfo& SetVSync(bool InVSync) { VSync = InVSync; return *this; }
	inline RHISwapchainCreateInfo& SetHDR(bool InHDR) { HDR = InHDR; return *this; }
};

struct RHIRenderPassCreateInfo2
{
	struct RHIAttachmentDesc
	{
		const RHITexture* Attachment = nullptr;
		RHISubresource Subresource = RHI::AllSubresource;

		ERHILoadOp LoadOp = ERHILoadOp::None;
		ERHIStoreOp StoreOp = ERHIStoreOp::None;

		ERHILoadOp StencilLoadOp = ERHILoadOp::None;
		ERHIStoreOp StencilStoreOp = ERHIStoreOp::None;
	};

	using AttachmentRef = uint32_t;

	struct RHISubpassDesc
	{
		enum class ERHIDepthStencilOp
		{
			None,
			Read,
			Write
		};

		enum class ERHIBindPoint
		{
			Graphics,
			Compute
		};

		std::vector<AttachmentRef> Inputs;
		std::vector<AttachmentRef> Outputs;

		ERHIBindPoint BindPoint = ERHIBindPoint::Graphics;
		ERHIDepthStencilOp DepthStencilOp = ERHIDepthStencilOp::None;
		RHIRenderPassCreateInfo2& RenderPassCreateInfo;

		RHISubpassDesc(ERHIBindPoint InBindPoint, RHIRenderPassCreateInfo2& InRenderPassCreateInfo)
			: BindPoint(InBindPoint)
			, RenderPassCreateInfo(InRenderPassCreateInfo)
		{
		}

		inline RHISubpassDesc& AddInput(const RHITexture* Texture)
		{
			return AddAttachment(Texture, Inputs);
		}

		inline RHISubpassDesc& AddOutput(const RHITexture* Texture)
		{
			return AddAttachment(Texture, Outputs);
		}

		inline RHISubpassDesc& SetBindPoint(ERHIBindPoint InBindPoint)
		{
			BindPoint = InBindPoint;
			return *this;
		}

		inline RHISubpassDesc& SetDepthStencilOp(ERHIDepthStencilOp InDepthStencilOp)
		{
			DepthStencilOp = InDepthStencilOp;
			return *this;
		}
	private:
		inline RHISubpassDesc& AddAttachment(const RHITexture* Texture, std::vector<AttachmentRef>& Attachments)
		{
			assert(Texture && RHI::IsColor(Texture->GetFormat()));
			auto It = RenderPassCreateInfo.ColorAttachemntIndices.find(Texture);
			assert(It != RenderPassCreateInfo.ColorAttachemntIndices.end());
			Attachments.emplace_back(It->second);
			return *this;
		}
	};

	std::vector<RHIAttachmentDesc> ColorAttachments;
	RHIAttachmentDesc DepthStencilAttachment;
	std::vector<RHISubpassDesc> Subpasses;

	std::unordered_map<const RHITexture*, AttachmentRef> ColorAttachemntIndices;

	inline RHIRenderPassCreateInfo2& AddColorAttachment(const RHITexture* Texture, 
		ERHILoadOp LoadOp = ERHILoadOp::Load, 
		ERHIStoreOp StoreOp = ERHIStoreOp::Store, 
		RHISubresource Subresource = RHI::AllSubresource)
	{
		assert(Texture && RHI::IsColor(Texture->GetFormat()));

		auto It = ColorAttachemntIndices.find(Texture);
		if (It == ColorAttachemntIndices.end())
		{
			It = ColorAttachemntIndices.insert(std::make_pair(Texture, static_cast<AttachmentRef>(ColorAttachments.size()))).first;
			ColorAttachments.emplace_back(RHIAttachmentDesc{ Texture, Subresource, LoadOp, StoreOp, ERHILoadOp::None, ERHIStoreOp::None });
		}
		return *this;
	}

	inline RHIRenderPassCreateInfo2& SetDepthStencilAttachment(const RHITexture* Texture,
		ERHILoadOp LoadOp = ERHILoadOp::Load,
		ERHIStoreOp StoreOp = ERHIStoreOp::Store,
		ERHILoadOp StencilLoadOp = ERHILoadOp::Load,
		ERHIStoreOp StencilStoreOp = ERHIStoreOp::Store,
		RHISubresource Subresource = RHI::AllSubresource)
	{
		assert(Texture && RHI::IsDepthStencil(Texture->GetFormat()));

		DepthStencilAttachment = RHIAttachmentDesc{ Texture, Subresource, LoadOp, StoreOp, StencilLoadOp, StencilStoreOp };
		return *this;
	}

	inline RHISubpassDesc& AddSubpass()
	{
		return Subpasses.emplace_back(RHISubpassDesc(RHISubpassDesc::ERHIBindPoint::Graphics, *this));
	}

	inline RHISubpassDesc& AddComputeSubpass()
	{
		return Subpasses.emplace_back(RHISubpassDesc(RHISubpassDesc::ERHIBindPoint::Compute, *this));
	}

	inline uint32_t GetNumSubpass() const { return Subpasses.empty() ? 1u : static_cast<uint32_t>(Subpasses.size()); }
};

class RHIRenderPass2 : public RHIResource
{
public:
	using RHIResource::RHIResource;
};