#pragma once

#include "Engine/RHI/RHIBuffer.h"
#include "Engine/RHI/RHIShader.h"
#include "Engine/RHI/RHIRenderStates.h"
#include "Engine/Asset/ShaderAsset.h"

struct RHIShaderResourceBinding
{
	uint32_t Binding = 0u;
	uint32_t DescriptorIndex = 0u;
	ERHIResourceType Type = ERHIResourceType::Unknown;

	union
	{
		RHISampler* Sampler;
		RHITexture* Texture;
		RHIBuffer* Buffer;
	};
};

using RHIShaderResourceLayout = std::array<std::vector<RHIShaderResourceBinding>, (size_t)ERHIShaderStage::Num>;

struct RHIGraphicsPipelineCreateInfo
{
	ERHIPrimitiveTopology PrimitiveTopology = ERHIPrimitiveTopology::TriangleList;
	RHIRasterizationStateCreateInfo RasterizationState;
	RHIBlendStateCreateInfo BlendState;
	RHIDepthStencilStateCreateInfo DepthStencilState;
	RHIMultisampleStateCreateInfo MultisampleState;
	RHIInputLayoutCreateInfo InputLayout;
	std::array<const ShaderAsset*, (size_t)ERHIShaderStage::Num> Shaders;

	inline RHIGraphicsPipelineCreateInfo& SetPrimitiveTopology(ERHIPrimitiveTopology InTopology) { PrimitiveTopology = InTopology; return *this; }
	inline RHIGraphicsPipelineCreateInfo& SetRasterizationState(const RHIRasterizationStateCreateInfo& InRasterizationState) { RasterizationState = InRasterizationState; return *this; }
	inline RHIGraphicsPipelineCreateInfo& SetBlendState(const RHIBlendStateCreateInfo& InBlendState) { BlendState = InBlendState; return *this; }
	inline RHIGraphicsPipelineCreateInfo& SetDepthStencilState(const RHIDepthStencilStateCreateInfo& InDepthStencilState) { DepthStencilState = InDepthStencilState; return *this; }
	inline RHIGraphicsPipelineCreateInfo& SetMultisampleState(const RHIMultisampleStateCreateInfo& InMultisampleState) { MultisampleState = InMultisampleState; return *this; }
	inline RHIGraphicsPipelineCreateInfo& SetShader(const ShaderAsset* Shader) { Shaders[static_cast<size_t>(Shader->GetStage())] = Shader; return *this; }
};

struct RHIComputePipelineCreateInfo
{
};

class RHIPipelineState
{
public:
	RHIPipelineState(const RHIGraphicsPipelineCreateInfo& GfxPipelineCreateInfo);

	bool IsDirty() const { return m_Dirty; }

	void Commit(RHICommandBuffer* CommandBuffer);

	template<ERHIShaderStage Stage>
	inline void SetBuffer(uint32_t Binding, RHIBuffer* Buffer)
	{
		size_t Index = static_cast<size_t>(Stage);
		MarkDirty(m_ShaderResourceLayout[Index][Binding].Buffer == Buffer);
		m_ShaderResourceLayout[Index][Binding].Buffer = Buffer;
	}

	const RHIShaderResourceLayout& GetShaderResourceLayout() const { return m_ShaderResourceLayout; }
protected:
	void MarkDirty(bool Dirty) { m_Dirty |= Dirty; }
	void ClearDirty() { m_Dirty = false; }

	virtual void CommitShaderResources() = 0;

	bool m_Dirty = false;
	RHIShaderResourceLayout m_ShaderResourceLayout;
};

#define ENABLE_SHADER_HOT_RELOAD 1

class RHIPipeline
{
public:
	RHIPipelineState* GetPipelineState() const { return m_PipelineState.get(); }
protected:
	std::shared_ptr<RHIPipelineState> m_PipelineState;
};

class RHIGraphicsPipeline : public RHIPipeline
{
#if ENABLE_SHADER_HOT_RELOAD
public:
	RHIGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& CreateInfo)
		: m_CreateInfo(CreateInfo)
	{
	}
private:
	RHIGraphicsPipelineCreateInfo m_CreateInfo;
#endif
};

class RHIComputePipeline : public RHIPipeline
{
};

template<class RHIDescription>
inline size_t ComputeHash(const RHIDescription&) { return 0ull; }

template<>
inline size_t ComputeHash(const RHIRasterizationStateCreateInfo& Desc)
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

template<>
inline size_t ComputeHash(const RHIBlendStateCreateInfo& Desc)
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

template<>
inline size_t ComputeHash(const RHIDepthStencilStateCreateInfo& Desc)
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

template<>
inline size_t ComputeHash(const RHIMultisampleStateCreateInfo& Desc)
{
	return ComputeHash(
		Desc.SampleCount,
		Desc.EnableSampleShading,
		Desc.EnableAlphaToCoverage,
		Desc.EnableAlphaToOne,
		Desc.MinSampleShading);
}

template<>
inline size_t ComputeHash(const RHIGraphicsPipelineCreateInfo& Desc)
{
	auto Hash = ComputeHash(
		ComputeHash(Desc.PrimitiveTopology),
		ComputeHash(Desc.RasterizationState),
		ComputeHash(Desc.BlendState),
		ComputeHash(Desc.DepthStencilState),
		ComputeHash(Desc.MultisampleState));
	for (auto Shader : Desc.Shaders)
	{
		if (Shader)
		{
			HashCombine(Hash, Shader);
		}
	}
	return Hash;
}
