#pragma once

#include "RHI/RHIBuffer.h"
#include "RHI/RHIShader.h"
#include "RHI/RHIRenderStates.h"
#include "Asset/Shader.h"

struct RHIShaderResourceBinding
{
	uint32_t Binding = 0u;
	uint32_t DescriptorIndex = 0u;
	ERHIResourceType Type = ERHIResourceType::Unknown;
	ERHIShaderStage Stage = ERHIShaderStage::Num;

	union
	{
		RHISampler* Sampler;
		RHITexture* Texture;
		RHIBuffer* Buffer;
	};
};

using RHIShaderResourceLayout = Array<std::vector<RHIShaderResourceBinding>, ERHIShaderStage>;

class RHIGraphicsShaderPipeline : public Array<std::shared_ptr<Shader>, ERHIShaderStage>
{
public:
	size_t ComputeHash() const;
};

struct RHIGraphicsPipelineDesc
{
	ERHIPrimitiveTopology PrimitiveTopology = ERHIPrimitiveTopology::TriangleList;
	RHIRasterizationStateDesc RasterizationState;
	RHIBlendStateDesc BlendState;
	RHIDepthStencilStateDesc DepthStencilState;
	RHIMultisampleStateDesc MultisampleState;
	RHIInputLayoutDesc InputLayout;
	std::vector<RHIViewport> Viewports;
	std::vector<RHIScissorRect> ScissorRects;
	RHIGraphicsShaderPipeline ShaderPipeline;

	inline RHIGraphicsPipelineDesc& SetPrimitiveTopology(ERHIPrimitiveTopology InTopology) { PrimitiveTopology = InTopology; return *this; }
	inline RHIGraphicsPipelineDesc& SetRasterizationState(const RHIRasterizationStateDesc& InRasterizationState) { RasterizationState = InRasterizationState; return *this; }
	inline RHIGraphicsPipelineDesc& SetBlendState(const RHIBlendStateDesc& InBlendState) { BlendState = InBlendState; return *this; }
	inline RHIGraphicsPipelineDesc& SetDepthStencilState(const RHIDepthStencilStateDesc& InDepthStencilState) { DepthStencilState = InDepthStencilState; return *this; }
	inline RHIGraphicsPipelineDesc& SetMultisampleState(const RHIMultisampleStateDesc& InMultisampleState) { MultisampleState = InMultisampleState; return *this; }
	inline RHIGraphicsPipelineDesc& SetShader(const std::shared_ptr<Shader>& InShader) { ShaderPipeline[InShader->GetStage()] = InShader; return *this; }
	
	inline RHIGraphicsPipelineDesc& SetViewport(const RHIViewport& Viewport)
	{
		if (Viewports.empty())
		{
			return AddViewport(Viewport);
		}

		Viewports[0] = Viewport;
		return *this;
	}

	inline RHIGraphicsPipelineDesc& AddViewport(const RHIViewport& Viewport) 
	{
		assert(Viewports.size() < ERHILimitations::MaxViewports);
		Viewports.emplace_back(Viewport);
		return *this;
	}

	inline RHIGraphicsPipelineDesc& SetScissorRect(const RHIScissorRect& ScissorRect)
	{
		if (ScissorRects.empty())
		{
			return AddScissorRect(ScissorRect);
		}

		ScissorRects[0] = ScissorRect;
		return *this;
	}

	inline RHIGraphicsPipelineDesc& AddScissorRect(const RHIScissorRect& ScissorRect)
	{
		assert(ScissorRects.size() < ERHILimitations::MaxScissorRects);
		ScissorRects.emplace_back(ScissorRect);
		return *this;
	}
};

struct RHIComputePipelineDesc
{
};

class RHIPipelineState
{
public:
	RHIPipelineState(const RHIGraphicsPipelineDesc& Desc);

	bool IsDirty() const { return m_PipelineStatesDirtyFlags.any() || m_ShaderResourceDirty; }

	void Commit(RHICommandBuffer* CommandBuffer);

	inline RHIPipelineState& SetBuffer(ERHIShaderStage Stage, uint32_t Binding, RHIBuffer* Buffer)
	{
		assert(Stage < ERHIShaderStage::Num);
		assert(Binding < m_ShaderResourceLayout[Stage].size());
		MarkDirty(m_ShaderResourceLayout[Stage][Binding].Buffer != Buffer);
		m_ShaderResourceLayout[Stage][Binding].Buffer = Buffer;
		
		return *this;
	}

	inline RHIPipelineState& SetTexture(ERHIShaderStage Stage, uint32_t Binding, RHITexture* Texture)
	{
		assert(Stage < ERHIShaderStage::Num);
		assert(Binding < m_ShaderResourceLayout[Stage].size());
		MarkDirty(m_ShaderResourceLayout[Stage][Binding].Texture != Texture);
		m_ShaderResourceLayout[Stage][Binding].Texture = Texture;
		
		return *this;
	}

	inline RHIPipelineState& SetSampler(ERHIShaderStage Stage, uint32_t Binding, RHISampler* Sampler)
	{
		assert(Stage < ERHIShaderStage::Num);
		assert(Binding < m_ShaderResourceLayout[Stage].size());
		MarkDirty(m_ShaderResourceLayout[Stage][Binding].Sampler != Sampler);
		m_ShaderResourceLayout[Stage][Binding].Sampler = Sampler;

		return *this;
	}

	const RHIShaderResourceLayout& GetShaderResourceLayout() const { return m_ShaderResourceLayout; }
protected:
	enum EDirtyFlagBits
	{
		Viewport,
		ScissorRect,
		VertexStream,
		IndexBuffer,
		FrameBuffer,
		Num
	};

	void MarkDirty(bool Dirty) { m_ShaderResourceDirty |= Dirty; }
	void MarkDirty(EDirtyFlagBits Bit, bool Dirty) { m_PipelineStatesDirtyFlags.set(Bit, Dirty); }
	void ClearDirty() { m_PipelineStatesDirtyFlags.reset(); m_ShaderResourceDirty = false; }
	bool IsPipelineStatesDirty() const { return m_PipelineStatesDirtyFlags.any(); }
	bool IsShaderResourceDirty() const { return m_ShaderResourceDirty; }

	virtual void CommitShaderResources(RHICommandBuffer* CommandBuffer) = 0;
	virtual void CommitPipelineStates(RHICommandBuffer* CommandBuffer) = 0;

	bool m_ShaderResourceDirty = false;
	std::bitset<EDirtyFlagBits::Num> m_PipelineStatesDirtyFlags;
	RHIShaderResourceLayout m_ShaderResourceLayout;
};

class RHIPipeline
{
public:
	RHIPipelineState* GetPipelineState() const { return m_PipelineState.get(); }
protected:
	std::shared_ptr<RHIPipelineState> m_PipelineState;
};

class RHIGraphicsPipeline : public RHIPipeline
{
#if SHADER_HOT_RELOAD
public:
	RHIGraphicsPipeline(const RHIGraphicsPipelineDesc& Desc)
		: m_Desc(Desc)
	{
	}
private:
	RHIGraphicsPipelineDesc m_Desc;
#endif
};

class RHIComputePipeline : public RHIPipeline
{
};

#define STD_HASHER(Description)                          \
namespace std                                            \
{                                                        \
	template<>                                           \
	struct hash<Description>                             \
	{                                                    \
		size_t operator()(const Description& Desc) const \
		{                                                \
			return ComputeHash<Description>(Desc);       \
		}                                                \
	};                                                   \
}

template<class RHIDescription>
inline size_t ComputeHash(const RHIDescription&) { return 0ull; }

template<>
inline size_t ComputeHash(const RHIRasterizationStateDesc& Desc)
{
	return ComputeHash(
		Desc.PolygonMode,
		Desc.CullMode,
		Desc.FrontFace,
		Desc.EnableDepthClamp,
		Desc.DepthBias,
		Desc.DepthBiasClamp,
		Desc.DepthBiasSlope,
		Desc.LineWidth);
}
STD_HASHER(RHIRasterizationStateDesc);

template<>
inline size_t ComputeHash(const RHIBlendStateDesc& Desc)
{
	auto Hash = ComputeHash(
		Desc.EnableLogicOp,
		Desc.LogicOp);
	for (uint32_t Index = 0u; Index < ERHILimitations::MaxRenderTargets; ++Index)
	{
		auto& BlendState = Desc.RenderTargetBlends[Index];
		HashCombine(Hash, 
			BlendState.Enable,
			BlendState.ColorMask,
			BlendState.SrcColor,
			BlendState.DstColor,
			BlendState.ColorOp,
			BlendState.SrcAlpha,
			BlendState.DstAlpha,
			BlendState.AlphaOp);
	}
	return Hash;
}
STD_HASHER(RHIBlendStateDesc);

template<>
inline size_t ComputeHash(const RHIStencilStateDesc& Desc)
{
	return ComputeHash(
		Desc.FailOp,
		Desc.PassOp,
		Desc.DepthFailOp,
		Desc.CompareFunc,
		Desc.Ref);
}
STD_HASHER(RHIStencilStateDesc);

template<>
inline size_t ComputeHash(const RHIDepthStencilStateDesc& Desc)
{
	return ComputeHash(
		Desc.EnableDepth,
		Desc.EnableDepthWrite,
		Desc.EnableDepthBoundsTest,
		Desc.EnableStencil,
		Desc.DepthCompareFunc,
		Desc.StencilReadMask,
		Desc.StencilWriteMask,
		Desc.DepthBounds.x,
		Desc.DepthBounds.y,
		ComputeHash(Desc.FrontFaceStencil),
		ComputeHash(Desc.BackFaceStencil));
}
STD_HASHER(RHIDepthStencilStateDesc);

template<>
inline size_t ComputeHash(const RHIMultisampleStateDesc& Desc)
{
	return ComputeHash(
		Desc.SampleCount,
		Desc.EnableSampleShading,
		Desc.EnableAlphaToCoverage,
		Desc.EnableAlphaToOne,
		Desc.MinSampleShading);
}
STD_HASHER(RHIMultisampleStateDesc);

template<>
inline size_t ComputeHash(const RHIRenderPassDesc& Desc)
{
	size_t Hash = ComputeHash(
		Desc.GetNumColorAttachments(),
		Desc.HasDepthStencil(),
		Desc.SampleCount);

	for (auto& Attachment : Desc.ColorAttachments)
	{
		HashCombine(Hash, Attachment.Format);
	}
	
	HashCombine(Hash, Desc.DepthStencilAttachment.Format);

	return Hash;
}
STD_HASHER(RHIRenderPassDesc);

template<>
inline size_t ComputeHash(const RHIGraphicsPipelineDesc& Desc)
{
	auto Hash = ComputeHash(
		ComputeHash(Desc.PrimitiveTopology),
		ComputeHash(Desc.RasterizationState),
		ComputeHash(Desc.BlendState),
		ComputeHash(Desc.DepthStencilState),
		ComputeHash(Desc.MultisampleState),
		ComputeHash(Desc.InputLayout));

	HashCombine(Hash, Desc.ShaderPipeline.ComputeHash());

	return Hash;
}
STD_HASHER(RHIGraphicsPipelineDesc);

template<>
inline size_t ComputeHash(const RHIFrameBufferDesc& Desc)
{
	auto Hash = ComputeHash(Desc.Width, Desc.Height, Desc.ArrayLayers);

	for (auto& Color : Desc.ColorAttachments)
	{
		HashCombine(Hash, Color.Texture);
	}

	if (Desc.HasDepthStencil())
	{
		HashCombine(Hash, Desc.DepthStencilAttachment.Texture);
	}

	return Hash;
}
STD_HASHER(RHIFrameBufferDesc);
