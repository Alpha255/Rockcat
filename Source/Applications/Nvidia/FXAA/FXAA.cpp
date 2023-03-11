#include "FXAA.h"
#include <D3DGraphic.h>
#include <Camera.h>
#include <D3DModel.h>

extern D3DGraphic* g_Renderer;

namespace FXAA
{
	enum eConstants
	{
		eGatherCount = 2,
		ePatternCount = 5,
		eFxaaPatternCount = 6,
		eShadowMapSize = 2048,
		eRandomRotSize = 16
	};

	struct D3DTextures
	{
		Ref<ID3D11Texture2D> ProxyTex;
		Ref<ID3D11Texture2D> DepthTex;
		Ref<ID3D11Texture2D> RandomRotTex;
		Ref<ID3D11Texture2D> CopyResolveTex;
	};

	struct D3DViews
	{
		Ref<ID3D11ShaderResourceView> ProxyTexSRV;
		Ref<ID3D11ShaderResourceView> DepthTexSRV;
		Ref<ID3D11ShaderResourceView> RandomRotTexSRV;
		Ref<ID3D11ShaderResourceView> CopyResolveTexSRV;

		Ref<ID3D11DepthStencilView> DepthTexDSV;
		Ref<ID3D11RenderTargetView> ProxyTexRTV;

		Ref<ID3D11RenderTargetView> EmptyRTV;
	};

	struct D3DShaders
	{
		Ref<ID3D11VertexShader> MainVS;
		Ref<ID3D11VertexShader> ShadowMapVS;
		Ref<ID3D11VertexShader> FxaaVS;
		Ref<ID3D11VertexShader> QuadVS;

		Ref<ID3D11PixelShader> MainPS[ePatternCount];
		Ref<ID3D11PixelShader> MainGatherPS[ePatternCount];
		Ref<ID3D11PixelShader> FxaaPS[eFxaaPatternCount];
		Ref<ID3D11PixelShader> EmptyPS;
		Ref<ID3D11PixelShader> QuadPS;
	};

	struct D3DStates
	{
		Ref<ID3D11BlendState> ColorWritesOn;
		Ref<ID3D11BlendState> ColorWritesOff;

		Ref<ID3D11RasterizerState> CullBack;
		Ref<ID3D11RasterizerState> CullFront;

		Ref<ID3D11RasterizerState> WireFrame;

		Ref<ID3D11SamplerState> PointMirror;    /// For rotation texture
		Ref<ID3D11SamplerState> LinearWrap;     /// For diffuse texture
		Ref<ID3D11SamplerState> PointCmpClamp;  /// Comparison sampler for shadowmap
		Ref<ID3D11SamplerState> Anisotropic;    /// Anisotropic sampler for FXAA
	};

	struct D3DConstantsBuffers
	{
		Ref<ID3D11Buffer> ShadowMap;
		Ref<ID3D11Buffer> Fxaa;
	};

	struct ConstantsShadowMap
	{
		Matrix WVP;
		Matrix WVPLight;

		Vec4 AmbientColor;
		Vec4 DiffuseColor;

		Vec4 LightPos;
		Vec4 LightDiffuseClr;

		float InvRandomRotSize;
		float InvShadowMapSize;
		float FilterWidth;
		float Padding;
	};

	struct ConstantsFxaa
	{
		Vec4 Fxaa;
	};

	static bool s_UseGather = false;
	static uint32_t s_CurPattern = 0U;
	static uint32_t s_FxaaPreset = 4U;
	static float s_Radius = 15.0f;
	static float s_Theta = 1.5f * DirectX::XM_PI;
	static float s_Phi = DirectX::XM_PIDIV4;

	static Camera s_DefaultCamera;
	static Camera s_LightCamera;

	static D3DTextures s_Textures;
	static D3DViews s_Views;
	static D3DShaders s_Shaders;
	static D3DStates s_States;
	static D3DConstantsBuffers s_ConstantsBuffers;
	static Ref<ID3D11InputLayout> s_InputLayout;
	static Ref<ID3D11InputLayout> s_LayoutShadowMap;
	static D3D11_VIEWPORT s_Viewport;
	static D3D11_RECT s_ScissorRect;

	static D3DModel s_CryptModel;
}

using namespace FXAA;

ApplicationFXAA::ApplicationFXAA()
{
	s_LightCamera.SetViewParams(Vec3(9.0f, 15.0f, 9.0f), Vec3(0.0f, 0.0f, 0.0f));
}

void ApplicationFXAA::CreateTextures()
{
	g_Renderer->CreateTexture2D(s_Textures.CopyResolveTex, DXGI_FORMAT_R8G8B8A8_UNORM, 
		m_Width, m_Height, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	g_Renderer->CreateTexture2D(s_Textures.ProxyTex, DXGI_FORMAT_R8G8B8A8_UNORM,
		m_Width, m_Height, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	/// Shadow map surfaces
	g_Renderer->CreateTexture2D(s_Textures.DepthTex, DXGI_FORMAT_R24G8_TYPELESS,
		eShadowMapSize, eShadowMapSize, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE);

	/// Random rotation texture surface 
	uint32_t texData[eRandomRotSize * eRandomRotSize] = { 0 };
	for (uint32_t i = 0; i < eRandomRotSize * eRandomRotSize; ++i)
	{
		float angle = (float)rand() / RAND_MAX * DirectX::XM_2PI;

		float cosAngle = cosf(angle);
		float sinAngle = sinf(angle);

		int x1 = (int)(127.0f * cosAngle);
		int y1 = (int)(127.0f * sinAngle);
		int x2 = (int)(-127.0f * sinAngle);
		int y2 = (int)(127.0f * cosAngle);

		texData[i] = (uint32_t)(((x1 & 0xff) << 24) | ((y1 & 0xff) << 16) | ((x2 & 0xff) << 8) | (y2 & 0xff));
	}
	g_Renderer->CreateTexture2D(s_Textures.RandomRotTex, DXGI_FORMAT_R8G8B8A8_SNORM,
		eRandomRotSize, eRandomRotSize, D3D11_BIND_SHADER_RESOURCE, 1U, 1U, 0U, 0U, D3D11_USAGE_DEFAULT, 
		texData, sizeof(uint32_t) * eRandomRotSize);
}

void ApplicationFXAA::CreateViews()
{
	assert(s_Textures.ProxyTex.Valid());
	g_Renderer->CreateRenderTargetView(s_Views.ProxyTexRTV, s_Textures.ProxyTex.Ptr(), nullptr);
	g_Renderer->CreateShaderResourceView(s_Views.ProxyTexSRV, s_Textures.ProxyTex.Ptr());

	assert(s_Textures.CopyResolveTex.Valid());
	g_Renderer->CreateShaderResourceView(s_Views.CopyResolveTexSRV, s_Textures.CopyResolveTex.Ptr());

	/// Shader resource view for shadowmap
	assert(s_Textures.DepthTex.Valid());
	g_Renderer->CreateShaderResourceView(s_Views.DepthTexSRV, s_Textures.DepthTex.Ptr(), DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
	g_Renderer->CreateDepthStencilView(s_Views.DepthTexDSV, s_Textures.DepthTex.Ptr(),
		DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_DSV_DIMENSION_TEXTURE2D);

	/// Shader resource view for random rotation texture
	assert(s_Textures.RandomRotTex.Valid());
	g_Renderer->CreateShaderResourceView(s_Views.RandomRotTexSRV, s_Textures.RandomRotTex.Ptr(), DXGI_FORMAT_R8G8B8A8_SNORM);
}

void ApplicationFXAA::CreateInputLayoutAndShaders()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	g_Renderer->CreateVertexShaderAndInputLayout(s_Shaders.ShadowMapVS, s_InputLayout,
		layout, ARRAYSIZE(layout), "Fxaa.hlsl", "VS_ShadowMap");

	g_Renderer->CreateVertexShader(s_Shaders.MainVS, "Fxaa.hlsl", "VS_Main");

	g_Renderer->CreateVertexShader(s_Shaders.FxaaVS, "Fxaa.hlsl", "VS_Fxaa");

	/// Pixel shaders
	static D3D_SHADER_MACRO PatternMacros[ePatternCount] =
	{
		{ "PATTERN", "44" },
		{ "PATTERN", "64" },
		{ "PATTERN", "38" },
		{ "PATTERN", "84" },
		{ "PATTERN", "48" }
	};
	static D3D_SHADER_MACRO GatherMacros[eGatherCount] =
	{
		{ "NOUSEGATHER4", "" },
		{ "USEGATHER4", "" }
	};
	static D3D_SHADER_MACRO PresetMacros[eFxaaPatternCount] =
	{
		{ "FXAA_PRESET", "0" },
		{ "FXAA_PRESET", "1" },
		{ "FXAA_PRESET", "2" },
		{ "FXAA_PRESET", "3" },
		{ "FXAA_PRESET", "4" },
		{ "FXAA_PRESET", "5" }
	};
	static D3D_SHADER_MACRO NullMacro = { nullptr, nullptr };

	std::vector<D3D_SHADER_MACRO> Macros;
	for (uint32_t pat = 0U; pat < ePatternCount; ++pat)
	{
		Macros.clear();
		Macros.push_back(PatternMacros[pat]);
		Macros.push_back(GatherMacros[0]);
		Macros.push_back(NullMacro);
		g_Renderer->CreatePixelShader(s_Shaders.MainPS[pat], "Fxaa.hlsl", "PS_Main", (const D3D_SHADER_MACRO*)&*Macros.begin());

		Macros.clear();
		Macros.push_back(PatternMacros[pat]);
		Macros.push_back(GatherMacros[1]);
		Macros.push_back(NullMacro);
		g_Renderer->CreatePixelShader(s_Shaders.MainGatherPS[pat], "Fxaa.hlsl", "PS_Main", (const D3D_SHADER_MACRO*)&*Macros.begin());
	}

	for (uint32_t pat = 0U; pat < eFxaaPatternCount; ++pat)
	{
		Macros.clear();
		Macros.push_back(PresetMacros[pat]);
		g_Renderer->CreatePixelShader(s_Shaders.FxaaPS[pat], "Fxaa.hlsl", "PS_Fxaa", (const D3D_SHADER_MACRO*)&*Macros.begin());
	}
}

void ApplicationFXAA::CreateMesh()
{
	s_CryptModel.CreateFromSDKMesh(L"crypt.sdkmesh");
}

void ApplicationFXAA::CreateStates()
{
	/// Sampler state
	D3D11_SAMPLER_DESC samDesc;
	memset(&samDesc, 0, sizeof(D3D11_SAMPLER_DESC));

	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samDesc.MipLODBias = 0.0f;
	samDesc.MaxAnisotropy = 1;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.BorderColor[0] = samDesc.BorderColor[1] = samDesc.BorderColor[2] = samDesc.BorderColor[3] = 0;
	samDesc.MinLOD = 0;
	samDesc.MaxLOD = D3D11_FLOAT32_MAX;
	g_Renderer->CreateSamplerState(s_States.PointMirror, &samDesc);

	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	g_Renderer->CreateSamplerState(s_States.LinearWrap, &samDesc);

	samDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	g_Renderer->CreateSamplerState(s_States.PointCmpClamp, &samDesc);

	samDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.MaxAnisotropy = 4;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.MaxLOD = 0.0f;
	samDesc.MinLOD = 0.0f;
	g_Renderer->CreateSamplerState(s_States.Anisotropic, &samDesc);

	/// Blend state
	D3D11_BLEND_DESC blendDesc;
	memset(&blendDesc, 0, sizeof(D3D11_BLEND_DESC));
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = false;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_Renderer->CreateBlendState(s_States.ColorWritesOn, &blendDesc);

	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0U;
	g_Renderer->CreateBlendState(s_States.ColorWritesOff, &blendDesc);

	/// Rasterizer state
	g_Renderer->CreateRasterizerState(s_States.CullBack, D3D11_FILL_SOLID, D3D11_CULL_BACK);

	g_Renderer->CreateRasterizerState(s_States.CullFront, D3D11_FILL_SOLID, D3D11_CULL_FRONT);

	g_Renderer->CreateRasterizerState(s_States.WireFrame, D3D11_FILL_WIREFRAME);
}

void ApplicationFXAA::CreateConstantsBuffers()
{
	g_Renderer->CreateConstantBuffer(s_ConstantsBuffers.ShadowMap, 
		sizeof(ConstantsShadowMap), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
	g_Renderer->CreateConstantBuffer(s_ConstantsBuffers.Fxaa, 
		sizeof(ConstantsFxaa), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
}

void ApplicationFXAA::SetupScene()
{
	CreateTextures();
	CreateViews();
	CreateInputLayoutAndShaders();
	CreateMesh();
	CreateStates();
	CreateConstantsBuffers();

	s_Viewport = { 0.0f, 0.0f, float(m_Width), float(m_Height), 0.0f, 1.0f };
	s_ScissorRect = { 0, 0, (long)m_Width, (long)m_Height };
}

void ApplicationFXAA::DrawDebugShadowMap()
{
	if (!s_LayoutShadowMap.Valid() && !s_Shaders.QuadVS.Valid())
	{
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		g_Renderer->CreateVertexShaderAndInputLayout(s_Shaders.QuadVS, s_LayoutShadowMap,
			layout, ARRAYSIZE(layout), "Quad.hlsl", "VSMain");
	}

	if (!s_Shaders.QuadPS.Valid())
	{
		g_Renderer->CreatePixelShader(s_Shaders.QuadPS, "Quad.hlsl", "PSMain");
	}

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetInputLayout(s_LayoutShadowMap);
	g_Renderer->SetVertexShader(s_Shaders.QuadVS);
	g_Renderer->SetPixelShader(s_Shaders.QuadPS);
	g_Renderer->SetShaderResource(s_Views.DepthTexSRV, 1U, D3DGraphic::ePixelShader);

	g_Renderer->DrawQuad(0.5f, 0.0f, m_Width / 8.0f, m_Height / 8.0f);
}

void ApplicationFXAA::DrawShadowMap()
{
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH, 1.0f, 0U);

	g_Renderer->SetConstantBuffer(s_ConstantsBuffers.ShadowMap, 0U, D3DGraphic::eVertexShader);
	g_Renderer->SetConstantBuffer(s_ConstantsBuffers.ShadowMap, 0U, D3DGraphic::ePixelShader);

	Matrix mLightWorldViewInv = (s_LightCamera.GetWorldMatrix() * s_LightCamera.GetViewMatrix()).Inverse();
	ConstantsShadowMap cbShadowMap;
	memset(&cbShadowMap, 0, sizeof(ConstantsShadowMap));
	cbShadowMap.LightDiffuseClr = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	cbShadowMap.WVP = s_DefaultCamera.GetWorldMatrix() * s_DefaultCamera.GetViewMatrix() * s_DefaultCamera.GetProjMatrix();
	cbShadowMap.WVPLight = s_LightCamera.GetWorldMatrix() * s_LightCamera.GetViewMatrix() * s_LightCamera.GetProjMatrix();
	cbShadowMap.LightPos = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cbShadowMap.LightPos.Transform(mLightWorldViewInv);
	cbShadowMap.AmbientColor = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
	cbShadowMap.DiffuseColor = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
	cbShadowMap.FilterWidth = 10.0f;
	cbShadowMap.InvRandomRotSize = 1.0f / eRandomRotSize;
	cbShadowMap.InvShadowMapSize = 1.0f / eShadowMapSize;
	g_Renderer->UpdateBuffer(s_ConstantsBuffers.ShadowMap, &cbShadowMap, sizeof(ConstantsShadowMap));

	D3D11_RECT shadowMapRect = { 0, eShadowMapSize, 0, eShadowMapSize };
	D3D11_VIEWPORT shadowMapViewport = { 0.0f, 0.0f, (float)eShadowMapSize, (float)eShadowMapSize, 0.0f, 1.0f };	
	g_Renderer->SetScissorRects(&shadowMapRect);
	g_Renderer->SetViewports(&shadowMapViewport);

	g_Renderer->SetRenderTarget(s_Views.EmptyRTV);
	g_Renderer->SetDepthStencil(s_Views.DepthTexDSV);
	g_Renderer->ClearDepthStencil(s_Views.DepthTexDSV, D3D11_CLEAR_DEPTH, 1.0f, 0U);

	g_Renderer->SetInputLayout(s_InputLayout);
	g_Renderer->SetVertexShader(s_Shaders.ShadowMapVS);
	g_Renderer->SetPixelShader(s_Shaders.EmptyPS);

	g_Renderer->SetBlendState(s_States.ColorWritesOff, Vec4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
	g_Renderer->SetRasterizerState(s_States.CullFront);

	///s_CryptModel.DrawCustom();

	g_Renderer->SetBlendState(s_States.ColorWritesOn, Vec4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
	g_Renderer->SetRasterizerState(s_States.CullBack);

	g_Renderer->SetScissorRects(&s_ScissorRect);
	g_Renderer->SetViewports(&s_Viewport);

	DrawDebugShadowMap();
}

void ApplicationFXAA::RenderScene()
{
	if (s_FxaaPreset)
	{
		g_Renderer->ClearRenderTarget(s_Views.ProxyTexRTV, nullptr);
	}
	else
	{
		g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget(), nullptr);
	}

	DrawShadowMap();

	//ConstantsFxaa cbFxaa;
	//memset(&cbFxaa, 0, sizeof(ConstantsFxaa));
	//cbFxaa.Fxaa = Vec4(1.0f / m_Width, 1.0f / m_Height, 0.0f, 0.0f);
	//g_Renderer->UpdateBuffer(s_ConstantsBuffers.Fxaa, &cbFxaa, sizeof(ConstantsFxaa));

	//g_Renderer->SetConstantBuffer(s_ConstantsBuffers.Fxaa, 1U, D3DGraphic::eVertexShader);
	//g_Renderer->SetConstantBuffer(s_ConstantsBuffers.Fxaa, 1U, D3DGraphic::ePixelShader);

	//if (s_FxaaPreset)
	//{
	//	g_Renderer->SetRenderTarget(s_Views.ProxyTexRTV);
	//}
	//else
	//{
	//	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	//}

	//g_Renderer->SetVertexShader(s_Shaders.MainVS);
	//if (s_UseGather)
	//{
	//	g_Renderer->SetPixelShader(s_Shaders.MainGatherPS[s_CurPattern]);
	//}
	//else
	//{
	//	g_Renderer->SetPixelShader(s_Shaders.MainPS[s_CurPattern]);
	//}

	//ID3D11SamplerState* ppSamplerStates[4] = {
	//	s_States.PointMirror,
	//	s_States.LinearWrap,
	//	s_States.PointCmpClamp,
	//	s_States.Anisotropic
	//};
	//g_Renderer->SetSamplerStates(ppSamplerStates, 0U, 4U);

	//ID3D11ShaderResourceView* ppShaderResourceViews[2] = {
	//	s_Views.DepthTexSRV,
	//	s_Views.RandomRotTexSRV
	//};
	//g_Renderer->SetShaderResource(ppShaderResourceViews, 1U, 2U);

	//s_CryptModel.DrawCustom();

	//ID3D11ShaderResourceView* pEmptySRV = nullptr;
	//g_Renderer->SetShaderResource(&pEmptySRV, 1U, 1U);

	//if (s_FxaaPreset)
	//{
	//	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	//	g_Renderer->SetVertexShader(s_Shaders.FxaaVS);
	//	g_Renderer->SetPixelShader(s_Shaders.FxaaPS[s_FxaaPreset - 1]);

	//	D3D11_TEXTURE2D_DESC backBufferDesc;
	//	g_Renderer->GetBackBufferDesc(backBufferDesc);
	//	if (backBufferDesc.SampleDesc.Count > 1U)
	//	{
	//		g_Renderer->ResolveSubResource(s_Textures.CopyResolveTex, s_Textures.ProxyTex, 0U, 0U, DXGI_FORMAT_R8G8B8A8_UNORM);
	//		g_Renderer->SetShaderResource(s_Views.CopyResolveTexSRV.Reference());
	//		g_Renderer->Draw(4U, 0U, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//	}
	//	else
	//	{
	//		g_Renderer->SetShaderResource(s_Views.ProxyTexSRV.Reference());
	//		g_Renderer->Draw(4U, 0U);
	//	}
	//}
}

void ApplicationFXAA::UpdateScene(float /*elapsedTime*/, float /*totalTime*/)
{
	float x = s_Radius * sinf(s_Phi) * cosf(s_Theta);
	float z = s_Radius * sinf(s_Phi) * sinf(s_Theta);
	float y = s_Radius * cosf(s_Phi);

	s_DefaultCamera.SetViewParams(Vec3(x, y, z), Vec3(0.0f, 0.0f, 0.0f));

	s_LightCamera.SetViewParams(Vec3(x, y, z), Vec3(0.0f, 0.0f, 0.0f));
}

void ApplicationFXAA::ResizeWindow(uint32_t width, uint32_t height)
{
	s_DefaultCamera.SetProjParams(DirectX::XM_PIDIV4, (float)width / height, 1.0f, 1000.0f);

	s_LightCamera.SetProjParams(DirectX::XM_PIDIV4, 1.0f, 0.1f, 1000.0f);

	Base::ResizeWindow(width, height);

	s_Viewport = { 0.0f, 0.0f, float(m_Width), float(m_Height), 0.0f, 1.0f };
	s_ScissorRect = { 0, 0, (long)m_Width, (long)m_Height };
}

void ApplicationFXAA::MouseMove(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) != 0)
	{
		float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos[0]));
		float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos[1]));

		s_Theta += dx;
		s_Phi += dy;

		s_Phi = Math::Clamp(s_Phi, 0.1f, DirectX::XM_PI - 0.1f);
	}
	else if ((wParam & MK_RBUTTON) != 0)
	{
		float dx = 0.0051f * static_cast<float>(x - m_LastMousePos[0]);
		float dy = 0.0051f * static_cast<float>(y - m_LastMousePos[1]);

		s_Radius += dx - dy;

		s_Radius = Math::Clamp(s_Radius, 3.0f, 15.0f);
	}

	m_LastMousePos[0] = x;
	m_LastMousePos[1] = y;
}