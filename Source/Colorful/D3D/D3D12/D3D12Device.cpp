#include "Colorful/D3D/D3D12/D3D12Device.h"

NAMESPACE_START(RHI)

D3D12Device::D3D12Device(DxgiFactory* Factory)
	: m_Adapter(std::move(std::make_unique<DxgiAdapter>(Factory)))
{
	assert(m_Adapter->IsValid());

	std::vector<D3D_FEATURE_LEVEL> FeatureLevels
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_1_0_CORE;
	for (auto Level : FeatureLevels)
	{
		if (SUCCEEDED(D3D12CreateDevice(m_Adapter->Get(), Level, IID_PPV_ARGS(Reference()))))
		{
			FeatureLevel = Level;
			break;
		}
	}

	if (IsValid())
	{
		DXGI_ADAPTER_DESC AdapterDesc;
		VERIFY_D3D(m_Adapter->Get()->GetDesc(&AdapterDesc));

		auto GetFeatureLevelName = [](D3D_FEATURE_LEVEL Level)->const char8_t* const
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
			}

			return "Unknown";
		};

		LOG_INFO("Create D3D12 device on adapter: \"{}\", DeviceID = {}. Feature Level = {}",
			StringUtils::ToMultiByte(AdapterDesc.Description),
			AdapterDesc.DeviceId,
			GetFeatureLevelName(FeatureLevel));

		D3D12_FEATURE_DATA_D3D12_OPTIONS FeatureOptions{};
		if (SUCCEEDED(m_Handle->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &FeatureOptions, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS))))
		{
			m_Features.ResourceBindingTier = FeatureOptions.ResourceBindingTier;
			m_Features.ResourceHeapTier = FeatureOptions.ResourceHeapTier;
		}

		D3D12_FEATURE_DATA_D3D12_OPTIONS1 FeatureOptions1{};
		if (SUCCEEDED(m_Handle->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS1, &FeatureOptions1, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS1))))
		{
			m_Features.WaveOps = FeatureOptions1.WaveOps;
		}

		D3D12_FEATURE_DATA_D3D12_OPTIONS2 FeatureOptions2{};
		if (SUCCEEDED(m_Handle->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS2, &FeatureOptions2, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS2))))
		{
		}

		D3D12_FEATURE_DATA_D3D12_OPTIONS3 FeatureOptions3{};
		if (SUCCEEDED(m_Handle->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS3, &FeatureOptions3, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS3))))
		{
		}

		D3D12_FEATURE_DATA_D3D12_OPTIONS4 FeatureOptions4{};
		if (SUCCEEDED(m_Handle->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS4, &FeatureOptions4, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS4))))
		{
		}

		D3D12_FEATURE_DATA_D3D12_OPTIONS5 FeatureOptions5{};
		if (SUCCEEDED(m_Handle->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &FeatureOptions5, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS5))))
		{
		}

		D3D12_FEATURE_DATA_D3D12_OPTIONS6 FeatureOptions6{};
		if (SUCCEEDED(m_Handle->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &FeatureOptions6, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS6))))
		{
			m_Features.ShadingRate = FeatureOptions6.AdditionalShadingRatesSupported;
			m_Features.ShadingRateTier = FeatureOptions6.VariableShadingRateTier;
		}

		D3D12_FEATURE_DATA_D3D12_OPTIONS7 FeatureOptions7{};
		if (SUCCEEDED(m_Handle->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &FeatureOptions7, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS7))))
		{
			m_Features.MeshShader = FeatureOptions7.MeshShaderTier == D3D12_MESH_SHADER_TIER_1;
		}

		D3D12_FEATURE_DATA_ROOT_SIGNATURE FeatureDataRootSignature{ D3D_ROOT_SIGNATURE_VERSION_1_1 };
		if (SUCCEEDED(m_Handle->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &FeatureDataRootSignature, sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE))))
		{
			m_Features.RootSignatureVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		}
		else
		{
			m_Features.RootSignatureVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		std::array<EQueueType, static_cast<size_t>(EQueueType::Compute) + 1ull> QueueTypes
		{
			EQueueType::Graphics,
			EQueueType::Transfer,
			EQueueType::Compute
		};
		for (uint32_t QueueIndex = 0u; QueueIndex < QueueTypes.size(); ++QueueIndex)
		{
			m_Queues[QueueIndex] = std::make_unique<D3D12CommandQueue>(Get(), QueueTypes[QueueIndex]);
		}

		for (uint32_t HeapIndex = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; HeapIndex < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++HeapIndex)
		{
			m_DescriptorAllocators[HeapIndex] = std::make_unique<D3D12DescriptorAllocator>(this, static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(HeapIndex));
		}
	}
	else
	{
		LOG_ERROR("Failed to create D3D12 device");
		assert(0);
	}
};

NAMESPACE_END(RHI)
