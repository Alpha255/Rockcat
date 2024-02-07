#include "RHI/D3D/DXGIInterface.h"
#include "Engine/RHI/RHIDevice.h"
#include "Engine/Services/SpdLogService.h"

namespace D3DResult
{
	const char* const ToString(::HRESULT Result)
	{
		/// https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d11-graphics-reference-returnvalues
		switch (Result)
		{
		case D3D11_ERROR_FILE_NOT_FOUND:
			return "The file was not found";
		case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
			return "There are too many unique instances of a particular type of state object";
		case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
			return "There are too many unique instances of a particular type of view object";
		case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD:
			return "The first call to ID3D11DeviceContext::Map after either "
				"ID3D11Device::CreateDeferredContext or ID3D11DeviceContext::FinishCommandList per Resource "
				"was not D3D11_MAP_WRITE_DISCARD";
		case DXGI_ERROR_INVALID_CALL:
			return "The method call is invalid. For example, a method's parameter may not be a valid pointer";
		case DXGI_ERROR_WAS_STILL_DRAWING:
			return "The previous blit operation that is transferring information to or from this surface is incomplete";
		case E_FAIL:
			return "Attempted to create a device with the debug layer enabled and the layer is not installed";
		case E_INVALIDARG:
			return "An invalid parameter was passed to the returning function";
		case E_OUTOFMEMORY:
			return "Direct3D could not allocate sufficient memory to complete the call";
		case E_NOTIMPL:
			return "The method call isn't implemented with the passed parameter combination";
		case D3D12_ERROR_ADAPTER_NOT_FOUND:
			return "The specified cached PSO was created on a different adapter and cannot be reused on the current adapter.";
		case D3D12_ERROR_DRIVER_VERSION_MISMATCH:
			return "The specified cached PSO was created on a different driver version and cannot be reused on the current adapter.";
		case S_FALSE:
			return "Alternate success value, indicating a successful but nonstandard completion (the precise meaning depends on context)";
		case S_OK:
			return "Successed";
		}

		return "Unknown";
	}
}

DxgiFactory::DxgiFactory(bool EnableDebug)
{
	uint32_t Flags = 0u;

#if defined(_DEBUG)
	if (EnableDebug)
	{
		Flags = DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	if (FAILED(CreateDXGIFactory2(Flags, IID_PPV_ARGS(Reference()))))
	{
		if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(Reference()))))
		{
			if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(Reference()))))
			{
				assert(0);
			}
		}
	}
}

DxgiAdapter::DxgiAdapter(const DxgiFactory& Factory)
{
	/// https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_6/nf-dxgi1_6-idxgifactory6-enumadapterbygpupreference

	uint32_t AdapterIndex = 0u;

	auto IsDedicateAdapter1 = [](DxgiAdapter1& Adapter1) -> bool {
		DXGI_ADAPTER_DESC1 Desc1;
		VERIFY_D3D(Adapter1->GetDesc1(&Desc1));

		if (Desc1.Flags & DXGI_ADAPTER_FLAG_REMOTE || Desc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE || Desc1.Flags & DXGI_ADAPTER_FLAG_FORCE_DWORD)
		{
			return false;
		}

		return true;
	};

	DxgiFactory6 Factory6;
	if (SUCCEEDED(Factory->QueryInterface(Factory6.Reference())))
	{
		DxgiAdapter1 Adapter1;
		while (SUCCEEDED(Factory6->EnumAdapterByGpuPreference(AdapterIndex++, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(Adapter1.Reference()))))
		{
			if (IsDedicateAdapter1(Adapter1))
			{
				new (this) DxgiAdapter(static_cast<IDXGIAdapter*>(Adapter1.GetNative()));
				break;
			}
		}
	}
	else
	{
		DxgiFactory1 Factory1;
		if (SUCCEEDED(Factory->QueryInterface(Factory1.Reference())))
		{
			DxgiAdapter1 Adapter1;
			while (SUCCEEDED(Factory1->EnumAdapters1(AdapterIndex++, Adapter1.Reference())))
			{
				if (IsDedicateAdapter1(Adapter1))
				{
					new (this) DxgiAdapter(static_cast<IDXGIAdapter*>(Adapter1.GetNative()));
					break;
				}
			}
		}
		else
		{
			DxgiAdapter0 Adapter0;
			while (SUCCEEDED(Factory->EnumAdapters(AdapterIndex++, Adapter0.Reference())))
			{
				DXGI_ADAPTER_DESC Desc;
				VERIFY_D3D(Adapter0->GetDesc(&Desc));

				if (RHIDevice::IsDedicatedDevice(Desc.VendorId))
				{
					break;
				}
			}
		}
	}
}
