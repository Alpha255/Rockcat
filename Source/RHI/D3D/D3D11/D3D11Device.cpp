#include "Colorful/D3D/D3D11/D3D11Device.h"

NAMESPACE_START(Gfx)

D3D11Device::D3D11Device(IDXGIAdapter1* adapter1, IDXGIAdapter4* adapter4)
{
	/// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdevice
	assert(adapter1 || adapter4);

	uint32_t flags = 0u;
#if defined(_DEBUG)
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	std::vector<D3D_FEATURE_LEVEL> featureLevels
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	std::vector<D3D_DRIVER_TYPE> driverTypes
	{
		D3D_DRIVER_TYPE_UNKNOWN,
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
		D3D_DRIVER_TYPE_NULL,
		D3D_DRIVER_TYPE_SOFTWARE,
		D3D_DRIVER_TYPE_WARP,
	};

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	for (uint32_t i = 0; i < driverTypes.size(); ++i)
	{
		ID3D11DeviceContext* IMContext = nullptr;
		if (SUCCEEDED(D3D11CreateDevice(
			adapter4 ? adapter4 : adapter1,
			(adapter4 || adapter1) ? D3D_DRIVER_TYPE_UNKNOWN : driverTypes[i],
			nullptr,
			flags,
			featureLevels.data(),
			(uint32_t)featureLevels.size(),
			D3D11_SDK_VERSION,
			reference(),
			&featureLevel,
			&IMContext)))
		{
			m_IMContext = std::make_unique<D3D11Context>(IMContext);
			break;
		}
	}
	assert(isValid());

	m_Device1 = queryAs<D3D11Device, D3D11Device1>(*this);
	m_Device2 = queryAs<D3D11Device, D3D11Device2>(*this);
	m_Device3 = queryAs<D3D11Device, D3D11Device3>(*this);
	m_Device4 = queryAs<D3D11Device, D3D11Device4>(*this);
	m_Device5 = queryAs<D3D11Device, D3D11Device5>(*this);

	uint32_t VendorID = ~0u;
	uint32_t DeviceID = ~0u;
	std::wstring DeviceName;

	if (adapter4)
	{
		DXGI_ADAPTER_DESC3 adapterDesc;
		VERIFY_D3D(adapter4->GetDesc3(&adapterDesc));
		VendorID = adapterDesc.VendorId;
		DeviceID = adapterDesc.DeviceId;
		DeviceName.assign(adapterDesc.Description);
	}
	else
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		VERIFY_D3D(adapter1->GetDesc1(&adapterDesc));
		VendorID = adapterDesc.VendorId;
		DeviceID = adapterDesc.DeviceId;
		DeviceName.assign(adapterDesc.Description);
	}

	auto getFeatureLevelName = [](D3D_FEATURE_LEVEL level)->const char8_t* const
	{
		switch (level)
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

	LOG_INFO("Created D3D11 device on adapter: \"%s\", DeviceID = %d. Feature Level = %s", DeviceName.c_str(), DeviceID, getFeatureLevelName(featureLevel));

	auto featureThreading = getFeatureData<D3D11_FEATURE_DATA_THREADING>(D3D11_FEATURE_THREADING);
	LOG_INFO("Device feature: \"DriverConcurrentCreates: %s\", \"DriverCommandLists: %s\"",
		featureThreading.DriverConcurrentCreates ? "True" : "False",
		featureThreading.DriverCommandLists ? "True" : "False");
}

D3D11Context::D3D11Context(ID3D11DeviceContext* handle)
	: D3DObject(handle)
{
	m_Context1 = queryAs<D3D11Context, D3D11Context1>(*this);
	m_Context2 = queryAs<D3D11Context, D3D11Context2>(*this);
	m_Context3 = queryAs<D3D11Context, D3D11Context3>(*this);
	m_Context4 = queryAs<D3D11Context, D3D11Context4>(*this);
}

NAMESPACE_END(Gfx)
