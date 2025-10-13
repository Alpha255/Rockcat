#include "RHI/D3D/D3D11/D3D11Device.h"

D3D11Device::D3D11Device(const DxgiFactory& Factory)
	: m_Adapter(std::move(std::make_unique<DxgiAdapter>(Factory)))
{
	assert(m_Adapter->IsValid());

	std::vector<D3D_FEATURE_LEVEL> FeatureLevels
	{
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	uint32_t Flags = 0u;
#if defined(_DEBUG)
	Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_1_0_CORE;
	for (auto Level : FeatureLevels)
	{
		if (SUCCEEDED(D3D11CreateDevice(
			m_Adapter->GetNative(), 
			D3D_DRIVER_TYPE_UNKNOWN,
			nullptr,
			Flags,
			&Level,
			1u, 
			D3D11_SDK_VERSION,
			Reference(),
			nullptr,
			nullptr)))
		{
			FeatureLevel = Level;
			break;
		}
	}

	if (IsValid())
	{
		DXGI_ADAPTER_DESC AdapterDesc;
		VERIFY_D3D(m_Adapter->GetNative()->GetDesc(&AdapterDesc));

		auto GetFeatureLevelName = [](D3D_FEATURE_LEVEL Level)->const char* const
		{
			switch (Level)
			{
			case D3D_FEATURE_LEVEL_9_1:  return "9.1";
			case D3D_FEATURE_LEVEL_9_2:  return "9.2";
			case D3D_FEATURE_LEVEL_9_3:  return "9.3";
			case D3D_FEATURE_LEVEL_10_0: return "10.0";
			case D3D_FEATURE_LEVEL_10_1: return "10.1";
			case D3D_FEATURE_LEVEL_11_0: return "11.0";
			case D3D_FEATURE_LEVEL_11_1: return "11.1";
			case D3D_FEATURE_LEVEL_12_0: return "12.0";
			case D3D_FEATURE_LEVEL_12_1: return "12.1";
			case D3D_FEATURE_LEVEL_12_2: return "12.2";
			default:
				return "Unknown";
			}
		};

		LOG_INFO("Create D3D11 device on adapter: \"{}\", DeviceID = {}. Feature Level = {}",
			StringUtils::ToMultiByte(AdapterDesc.Description),
			AdapterDesc.DeviceId,
			GetFeatureLevelName(FeatureLevel));

		D3D11_FEATURE_DATA_D3D11_OPTIONS FeatureOptions{};
		if (SUCCEEDED(GetNative()->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS, &FeatureOptions, sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS))))
		{
		}

		D3D11_FEATURE_DATA_D3D11_OPTIONS1 FeatureOptions1{};
		if (SUCCEEDED(GetNative()->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, &FeatureOptions1, sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS1))))
		{
		}

		D3D11_FEATURE_DATA_D3D11_OPTIONS2 FeatureOptions2{};
		if (SUCCEEDED(GetNative()->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS2, &FeatureOptions2, sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS2))))
		{
		}

		D3D11_FEATURE_DATA_D3D11_OPTIONS3 FeatureOptions3{};
		if (SUCCEEDED(GetNative()->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS3, &FeatureOptions3, sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS3))))
		{
		}

		D3D11_FEATURE_DATA_D3D11_OPTIONS4 FeatureOptions4{};
		if (SUCCEEDED(GetNative()->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS4, &FeatureOptions4, sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS4))))
		{
		}

		D3D11_FEATURE_DATA_D3D11_OPTIONS5 FeatureOptions5{};
		if (SUCCEEDED(GetNative()->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS5, &FeatureOptions5, sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS5))))
		{
		}

		const std::array<ERHIDeviceQueue, static_cast<size_t>(ERHIDeviceQueue::Num)> QueueTypes
		{
			ERHIDeviceQueue::Graphics,
			ERHIDeviceQueue::Transfer,
			ERHIDeviceQueue::Compute
		};
		for (uint32_t QueueIndex = 0u; QueueIndex < QueueTypes.size(); ++QueueIndex)
		{
			//m_Queues[QueueIndex] = std::make_unique<D3D11CommandQueue>(Get(), QueueTypes[QueueIndex]);
		}
	}
	else
	{
		LOG_ERROR("Failed to create D3D11 device");
		assert(0);
	}
}

RHIShaderPtr D3D11Device::CreateShader(const RHIShaderDesc& Desc) const
{
	return RHIShaderPtr();
}

RHITexturePtr D3D11Device::CreateTexture(const RHITextureDesc& Desc) const
{
	return RHITexturePtr();
}

RHIInputLayoutPtr D3D11Device::CreateInputLayout(const RHIInputLayoutDesc& Desc) const
{
	return RHIInputLayoutPtr();
}

RHIFrameBufferPtr D3D11Device::CreateFrameBuffer(const RHIFrameBufferDesc& Desc) const
{
	return RHIFrameBufferPtr();
}

RHIGraphicsPipelinePtr D3D11Device::CreateGraphicsPipeline(const RHIGraphicsPipelineDesc& Desc) const
{
	return RHIGraphicsPipelinePtr();
}

RHIPipelineStatePtr D3D11Device::CreatePipelineState(const RHIGraphicsPipelineDesc& Desc) const
{
	return RHIPipelineStatePtr();
}

RHIBufferPtr D3D11Device::CreateBuffer(const RHIBufferDesc& Desc) const
{
	return RHIBufferPtr();
}

RHISamplerPtr D3D11Device::CreateSampler(const RHISamplerDesc& Desc) const
{
	return RHISamplerPtr();
}

RHISwapchainPtr D3D11Device::CreateSwapchain(const RHISwapchainDesc& Desc) const
{
	return nullptr;
}

RHICommandListContext* D3D11Device::GetImmediateCommandListContext(ERHIDeviceQueue Queue)
{
	return nullptr;
}

RHICommandListContextPtr D3D11Device::AcquireDeferredCommandListContext()
{
	return RHICommandListContextPtr();
}

void D3D11Device::ReleaseDeferredCommandListContext(RHICommandListContextPtr& CmdListContext)
{
}
