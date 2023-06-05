#pragma once

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_2.h>
#include <d3d11_3.h>
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include "Runtime/Engine/RHI/RHIResource.h"

template<class HwObjectType>
class D3DHwResource : public RHIObject<HwObjectType>, public RHIResource
{
public:
	D3DHwResource() = default;

	D3DHwResource(HwObjectType* Handle, bool8_t AddRef = true)
		: RHIResource<HwObjectType>(Handle)
	{
		if (AddRef)
		{
			Handle->AddRef();
		}
	}

	D3DHwResource(const D3DHwResource& Other)
	{
		if (m_Handle != Other.m_Handle)
		{
			if (m_Handle)
			{
				m_Handle->Release();
			}

			if (Other.m_Handle)
			{
				Other.m_Handle->AddRef();
			}

			m_Handle = Other.m_Handle;
		}
	}

	D3DHwResource(D3DHwResource&& Other)
	{
		if (m_Handle != Other.m_Handle)
		{
			if (m_Handle)
			{
				m_Handle->Release();
			}

			m_Handle = std::exchange(Other.m_Handle, {});
		}
	}

	D3DHwResource& operator=(const D3DHwResource& Other)
	{
		if (m_Handle != Other.m_Handle)
		{
			if (m_Handle)
			{
				m_Handle->Release();
			}

			if (Other.m_Handle)
			{
				Other.m_Handle->AddRef();
			}

			m_Handle = Other.m_Handle;
		}

		return *this;
	}

	D3DHwResource& operator=(D3DHwResource&& Other) noexcept
	{
		if (m_Handle != Other.m_Handle)
		{
			if (m_Handle)
			{
				m_Handle->Release();
			}

			m_Handle = std::exchange(Other.m_Handle, {});
		}

		return *this;
	}

	void SetDebugName(const char8_t* Name) override final
	{
		assert(name && m_Handle);
		m_Handle->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<uint32_t>(std::strlen(Name)), Name);
	}

	virtual ~D3DHwResource()
	{
		if (m_Handle)
		{
			m_Handle->Release();
			m_Handle = nullptr;
		}
	}
protected:
private:
};

namespace D3DResult
{
	const char8_t* const ToString(::HRESULT Result);
};

#define VERIFY_D3D(Func)                                                       \
{                                                                              \
	::HRESULT Result = (Func);                                                 \
	if (FAILED(Result))                                                        \
	{                                                                          \
		LOG_ERROR("Failed to invoke D3DAPI, {}", D3DResult::ToString(Result)); \
		assert(0);                                                             \
	}                                                                          \
}

#define DECLARE_D3D_HWOBJECT(ClassName, THWInterface)    \
class ClassName : public D3DHWObject<void, THWInterface> \
{                                                        \
public:                                                  \
	using D3DHWObject::D3DHWObject;                      \
};

DECLARE_D3D_HWOBJECT(DxgiFactory0, IDXGIFactory)
DECLARE_D3D_HWOBJECT(DxgiFactory1, IDXGIFactory1)
DECLARE_D3D_HWOBJECT(DxgiFactory2, IDXGIFactory2)
DECLARE_D3D_HWOBJECT(DxgiFactory3, IDXGIFactory3)
DECLARE_D3D_HWOBJECT(DxgiFactory4, IDXGIFactory4)
DECLARE_D3D_HWOBJECT(DxgiFactory5, IDXGIFactory5)
DECLARE_D3D_HWOBJECT(DxgiFactory6, IDXGIFactory6)
DECLARE_D3D_HWOBJECT(DxgiFactory7, IDXGIFactory7)

DECLARE_D3D_HWOBJECT(DxgiAdapter0, IDXGIAdapter)
DECLARE_D3D_HWOBJECT(DxgiAdapter1, IDXGIAdapter1)
DECLARE_D3D_HWOBJECT(DxgiAdapter2, IDXGIAdapter2)
DECLARE_D3D_HWOBJECT(DxgiAdapter3, IDXGIAdapter3)
DECLARE_D3D_HWOBJECT(DxgiAdapter4, IDXGIAdapter4)

DECLARE_D3D_HWOBJECT(DxgiSwapChain0, IDXGISwapChain)
DECLARE_D3D_HWOBJECT(DxgiSwapChain1, IDXGISwapChain1)
DECLARE_D3D_HWOBJECT(DxgiSwapChain2, IDXGISwapChain2)
DECLARE_D3D_HWOBJECT(DxgiSwapChain3, IDXGISwapChain3)
DECLARE_D3D_HWOBJECT(DxgiSwapChain4, IDXGISwapChain4)

DECLARE_D3D_HWOBJECT(DxgiOutput0, IDXGIOutput)
DECLARE_D3D_HWOBJECT(DxgiOutput1, IDXGIOutput1)
DECLARE_D3D_HWOBJECT(DxgiOutput2, IDXGIOutput2)
DECLARE_D3D_HWOBJECT(DxgiOutput3, IDXGIOutput3)
DECLARE_D3D_HWOBJECT(DxgiOutput4, IDXGIOutput4)
DECLARE_D3D_HWOBJECT(DxgiOutput5, IDXGIOutput5)
DECLARE_D3D_HWOBJECT(DxgiOutput6, IDXGIOutput6)

DECLARE_D3D_HWOBJECT(D3DBlob, ID3DBlob)

/// https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_3/nf-dxgi1_3-createdxgifactory2
class DxgiFactory : public DxgiFactory0
{
public:
	DxgiFactory(bool8_t EnableDebug);
};

class DxgiAdapter : public DxgiAdapter0
{
public:
	using DxgiAdapter0::DxgiAdapter0;
	DxgiAdapter(DxgiFactory* Factory);
};

inline uint32_t CalcSubresource(uint32_t MipSlice, uint32_t ArraySlice, uint32_t PlaneSlice, uint32_t MipLevels, uint32_t ArraySize)
{
	return MipSlice + (ArraySlice * MipLevels) + (PlaneSlice * MipLevels * ArraySize);
}

NAMESPACE_END(RHI)
