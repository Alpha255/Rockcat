#pragma once

#include "Engine/RHI/RHIFormat.h"
#include <d3d11.h>

enum ERHILimitations : uint8_t
{
	MaxRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
	MaxViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
	MaxScissorRects = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
	MaxSamplers = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT,
	MaxUniformBuffers = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT,
	MaxImages = D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT,
	MaxRWBufferss = D3D11_1_UAV_SLOT_COUNT,
	MaxVertexStreams = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT
};

enum class ERHIIndexFormat : uint8_t
{
	UInt16 = sizeof(uint16_t),
	UInt32 = sizeof(uint32_t)
};

enum class ERHIResourceType : uint8_t
{
	Unknown,
	Sampler,
	CombinedImageSampler,
	SampledImage,
	StorageImage,
	UniformTexelBuffer,
	StorageTexelBuffer,
	UniformBuffer,
	StorageBuffer,
	UniformBufferDynamic,
	StorageBufferDynamic,
	InputAttachment,
	PushConstants
};

enum class ERHIDeviceAccessFlags : uint8_t
{
	None,
	GpuRead = 1 << 0,
	GpuReadWrite = 1 << 1,
	CpuRead = 1 << 2,
	CpuWrite = 1 << 3,
	GpuReadCpuWrite = 1 << 4
};
ENUM_FLAG_OPERATORS(ERHIDeviceAccessFlags)

enum class ERHIBufferUsageFlags : uint16_t
{
	None,
	VertexBuffer = 1 << 0,
	IndexBuffer = 1 << 1,
	UniformBuffer = 1 << 2,
	ShaderResource = 1 << 3,
	StreamOutput = 1 << 4,
	RenderTarget = 1 << 5,
	DepthStencil = 1 << 6,
	UnorderedAccess = 1 << 7,
	IndirectBuffer = 1 << 8,
	InputAttachment = 1 << 9,
	ByteAddressBuffer = 1 << 10,
	StructuredBuffer = 1 << 12,
	AccelerationStructure = 1 << 13
};
ENUM_FLAG_OPERATORS(ERHIBufferUsageFlags)

enum class ERHIResourceState : uint32_t
{
	Unknown = 0xFFFFFFFF,
	Common = 0x0,
	VertexBuffer = 0x1,
	UniformBuffer = 0x2,
	IndexBuffer = 0x4,
	RenderTarget = 0x8,
	UnorderedAccess = 0x10,
	DepthWrite = 0x20,
	DepthRead = 0x40,
	StreamOut = 0x80,
	IndirectArgument = 0x100,
	TransferDst = 0x200,
	TransferSrc = 0x400,
	ResolveDst = 0x800,
	ResolveSrc = 0x1000,
	AccelerationStructure = 0x2000,
	ShadingRate = 0x4000,
	ShaderResource = 0x8000,
	Present = 0x10000,
	InputAttachment = 0x20000
};

enum class ERHIDebugLayerLevel
{
	None,
	Info,
	Warning,
	Error,
	Verbose
};

template<class T>
class RHIObject
{
public:
	using Type = std::remove_extent_t<T>;

	RHIObject() = default;

	explicit RHIObject(Type* Handle)
		: m_Handle(Handle)
	{
	}

	RHIObject(const RHIObject& Other)
		: m_Handle(Other.m_Handle)
	{
	}

	RHIObject(RHIObject&& Other)
		: m_Handle(Other.m_Handle)
	{
		std::exchange(Other.m_Handle, {});
	}

	RHIObject& operator=(const RHIObject& Other)
	{
		if (m_Handle != Other.m_Handle)
		{
			m_Handle = Other.m_Handle;
		}
		return *this;
	}

	RHIObject& operator=(RHIObject&& Other)
	{
		if (m_Handle != Other.m_Handle)
		{
			m_Handle = std::exchange(Other.m_Handle, {});
		}
		return *this;
	}

	bool operator()() const { return IsValid(); }

	virtual ~RHIObject() { std::exchange(m_Handle, {}); }

	inline T** Reference() { return &m_Handle; }
	inline const T** Reference() const { return &m_Handle; }

	inline T* Get() { assert(m_Handle); return m_Handle; }
	inline const T* const Get() const { assert(m_Handle); return m_Handle;}

	inline T* operator->() { assert(m_Handle); return m_Handle; }
	inline T* operator->() const { assert(m_Handle); return m_Handle; }

	inline const bool IsValid() const { return m_Handle != nullptr; }

	inline intptr_t ToIntPtr() const { return reinterpret_cast<intptr_t>(m_Handle); }
protected:
	T* m_Handle = nullptr;
};

class RHIResource
{
public:
	RHIResource() = default;
	virtual ~RHIResource() = default;

	RHIResource(const char* DebugName)
		: m_DebugName(DebugName)
	{
	}

	virtual void SetDebugName(const char* DebugName) { m_DebugName = DebugName; }
	inline const char* GetDebugName() const { return m_DebugName.c_str(); }
private:
	std::string m_DebugName;
};
