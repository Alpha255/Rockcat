#pragma once

#include "Runtime/Engine/RHI/RHIBuffer.h"
#include "Runtime/Engine/RHI/RHIShader.h"

struct RHIShaderVariableBinding
{
	ERHIShaderStage Stage;
	uint32_t Binding = 0u;
	ERHIResourceType Type = ERHIResourceType::UniformBuffer;
};
using RHIPipelineLayoutDesc = std::vector<RHIShaderVariableBinding>;

struct RHIGraphicsPipelineCreateInfo
{
	ERHIPrimitiveTopology PrimitiveTopology = ERHIPrimitiveTopology::TriangleList;
	RHIRasterizationStateCreateInfo RasterizationState;
	RHIBlendStateCreateInfo BlendState;
	RHIDepthStencilStateCreateInfo DepthStencilState;
	RHIMultisampleStateCreateInfo MultisampleState;

	std::array<RHIShaderCreateInfo, (size_t)ERHIShaderStage::Num> Shaders;

	inline RHIGraphicsPipelineCreateInfo& SetPrimitiveTopology(ERHIPrimitiveTopology Topology) { PrimitiveTopology = Topology; return *this; }
	
	inline RHIGraphicsPipelineCreateInfo& SetShader(RHIShaderCreateInfo&& CreateInfo) 
	{ 
		assert(CreateInfo.Stage < ERHIShaderStage::Num);
		Shaders[static_cast<size_t>(CreateInfo.Stage)] = std::move(CreateInfo);
		return *this;
	}
};

struct RHIComputePipelineCreateInfo
{
};

struct RHIPipelineState
{
#if 0
public:
	enum class EDirtyFlags : uint8_t
	{
		Resources,
		VertexBuffer,
		IndexBuffer,
		Viewport,
		ScissorRect,
		PolygonMode,
		Max
	};

	PipelineState(const GraphicsPipelineDesc& Desc);

	virtual ~PipelineState() = default;

	template<EShaderStage Stage>
	void SetImage(uint32_t Binding, const IImageSharedPtr& Image)
	{
		if (auto Variable = ShaderVariableTable.Find(Stage, Binding))
		{
			if (Variable->Get() != Image.get())
			{
				assert(Variable->ShaderStage() == Stage);
				assert(Variable->ResourceType() == EResourceType::SampledImage || Variable->ResourceType() == EResourceType::StorageImage);

				SetDirty<EDirtyFlags::Resources>(true);
				Variable->Set(Image);
				WriteImage(Stage, Binding, Image.get());
			}
		}
	}

	template<EShaderStage Stage>
	void SetSampler(uint32_t Binding, const ISamplerSharedPtr& Sampler)
	{
		if (auto Variable = ShaderVariableTable.Find(Stage, Binding))
		{
			if (Variable->Get() != Sampler.get())
			{
				assert(Variable->ShaderStage() == Stage);
				assert(Variable->ResourceType() == EResourceType::Sampler);

				SetDirty<EDirtyFlags::Resources>(true);
				Variable->Set(Sampler);
				WriteSampler(Stage, Binding, Sampler.get());
			}
		}
	}

	template<EShaderStage Stage>
	void SetUniformBuffer(uint32_t Binding, const IBufferSharedPtr& Buffer)
	{
		if (auto Variable = ShaderVariableTable.Find(Stage, Binding))
		{
			if (Variable->Get() != Buffer.get())
			{
				assert(Variable->ShaderStage() == Stage);
				assert(Variable->ResourceType() == EResourceType::UniformBuffer);

				SetDirty<EDirtyFlags::Resources>(true);
				Variable->Set(Buffer);
				WriteUniformBuffer(Stage, Binding, Buffer.get());
			}
		}
	}

	void SetVertexBuffer(IBuffer* Buffer)
	{
		if (VertexBuffer != Buffer)
		{
			SetDirty<EDirtyFlags::VertexBuffer>(true);
			VertexBuffer = Buffer;
		}
	}

	void SetIndexBuffer(IBuffer* Buffer, EIndexFormat Format)
	{
		if (IndexBuffer.first != Buffer)
		{
			SetDirty<EDirtyFlags::IndexBuffer>(true);
			IndexBuffer = std::make_pair(Buffer, Format);
		}
	}

	void SetPolygonMode(EPolygonMode Mode)
	{
		if (PolygonMode != Mode)
		{
			SetDirty<EDirtyFlags::PolygonMode>(true);
			PolygonMode = Mode;
		}
	}

	void SetViewport(const Viewport& InViewport)
	{
		if (Viewport != InViewport)
		{
			SetDirty<EDirtyFlags::Viewport>(true);
			Viewport = InViewport;
		}
	}

	void SetScissorRect(const ScissorRect& Rect)
	{
		if (Scissor != Rect)
		{
			SetDirty<EDirtyFlags::ScissorRect>(true);
			Scissor = Rect;
		}
	}

	const PipelineShaderVariableTable& GetShaderVariableTable() const
	{
		return ShaderVariableTable;
	}

	template<EDirtyFlags Flags>
	const bool8_t IsDirty() const
	{
		return Dirty[static_cast<size_t>(Flags)];
	}

	template<EDirtyFlags Flags>
	void SetDirty(bool8_t IsDirty)
	{
		Dirty.set(static_cast<size_t>(Flags), IsDirty);
	}

	void Reset()
	{
		Dirty.reset();
	}
protected:
	virtual void WriteImage(EShaderStage Stage, uint32_t Binding, IImage* Image) = 0;
	virtual void WriteSampler(EShaderStage Stage, uint32_t Binding, ISampler* Sampler) = 0;
	virtual void WriteUniformBuffer(EShaderStage Stage, uint32_t Binding, IBuffer* Buffer) = 0;
public:
	IBuffer* VertexBuffer = nullptr;
	std::pair<IBuffer*, EIndexFormat> IndexBuffer;
	Viewport Viewport;
	ScissorRect Scissor;
	EPolygonMode PolygonMode = EPolygonMode::Solid;
	IFrameBuffer* FrameBuffer = nullptr;
private:
	std::bitset<static_cast<size_t>(EDirtyFlags::Max)> Dirty;
	PipelineShaderVariableTable ShaderVariableTable;
#endif
};

class RHIDescriptorSet
{
public:
	virtual void Commit(const RHIPipelineLayoutDesc& Desc) = 0;
};

class RHIPipeline
{
};

class RHIGraphicsPipeline : public RHIPipeline
{
};

class RHIComputePipeline : public RHIPipeline
{
};
