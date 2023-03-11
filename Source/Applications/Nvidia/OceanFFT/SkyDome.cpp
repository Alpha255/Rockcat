#include "SkyDome.h"

#include "Camera.h"

extern D3DGraphic* g_Renderer;

void SkyDome::RecreateVertexBuffer(uint32_t width, uint32_t height)
{
	static Ref<ID3D11Buffer> NullBuffer;

	m_Res.VertexBuffer = NullBuffer;

	/// Map texels to pixels
	float highW = -1.0f - 1.0f / width;
	float highH = -1.0f - 1.0f / height;
	float lowW = -highW;
	float lowH = -highH;

	Vec4 vertices[4];
	vertices[0] = { lowW, lowH, 1.0f, 1.0f };
	vertices[1] = { lowW, highH, 1.0f, 1.0f };
	vertices[2] = { highW, lowH, 1.0f, 1.0f };
	vertices[3] = { highW, highH, 1.0f, 1.0f };
	g_Renderer->CreateVertexBuffer(m_Res.VertexBuffer, sizeof(Vec4) * 4U, D3D11_USAGE_IMMUTABLE, vertices);
}

void SkyDome::Create(uint32_t width, uint32_t height)
{
	assert(g_Renderer && !m_bInited);

	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const char *c_ShaderFile = "SkyDome.hlsl";
	g_Renderer->CreateVertexShaderAndInputLayout(m_Res.VertexShader, m_Res.InputLayout, layout,
		ARRAYSIZE(layout), c_ShaderFile, "VSMain");
	g_Renderer->CreatePixelShader(m_Res.PixelShader, c_ShaderFile, "PSMain");

	RecreateVertexBuffer(width, height);

	g_Renderer->CreateConstantBuffer(m_Res.ConstantsBuffer, sizeof(Matrix), D3D11_USAGE_DYNAMIC,
		nullptr, D3D11_CPU_ACCESS_WRITE);

	g_Renderer->CreateShaderResourceView(m_Res.Cubemap, "sky_cube.dds");

	g_Renderer->CreateSamplerState(m_Res.LinearSampler, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, 0.0f, D3D11_COMPARISON_ALWAYS);

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	memset(&dsDesc, 0, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = false;
	g_Renderer->CreateDepthStencilState(m_Res.DisableDepthStencil, &dsDesc);

	m_bInited = true;
}

void SkyDome::Resize(uint32_t width, uint32_t height)
{
	if (!g_Renderer)
	{
		return;
	}

	RecreateVertexBuffer(width, height);

	D3D11_VIEWPORT vp{ 0 };
	vp.Width = (float)width;
	vp.Height = (float)height;
	g_Renderer->SetViewports(&vp);
}

void SkyDome::Draw(const Camera &cam)
{
	Matrix world
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	Matrix view = cam.GetViewMatrix();
	Matrix proj = cam.GetProjMatrix();
	Matrix wvpTranspose = (world * view * proj).Transpose();
	g_Renderer->UpdateBuffer(m_Res.ConstantsBuffer, &wvpTranspose, sizeof(Matrix));

	g_Renderer->SetInputLayout(m_Res.InputLayout);
	g_Renderer->SetVertexBuffer(m_Res.VertexBuffer, sizeof(Vec4), 0U);
	
	g_Renderer->SetVertexShader(m_Res.VertexShader);
	g_Renderer->SetPixelShader(m_Res.PixelShader);

	g_Renderer->SetShaderResource(m_Res.Cubemap, 0U, D3DGraphic::ePixelShader);

	g_Renderer->SetConstantBuffer(m_Res.ConstantsBuffer, 0U, D3DGraphic::eVertexShader);

	g_Renderer->SetSamplerStates(m_Res.LinearSampler, 0U, D3DGraphic::ePixelShader);
	
	g_Renderer->SetDepthStencilState(m_Res.DisableDepthStencil, 0U);

	g_Renderer->Draw(4U, 0U, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

