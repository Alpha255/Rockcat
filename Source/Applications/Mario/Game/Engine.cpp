#include "Engine.h"

#include "Image.h"
#include "Objects.h"
#include "ObjectMario.h"
#include "Map.h"
#include "D3DEngine.h"
#include "GameApplication.h"

std::unique_ptr<Engine, std::function<void(Engine *)>> Engine::s_Instance;

void Engine::LoadTextures()
{
	static const char *const s_TexNames[Object2D::eTypeCount] = 
	{
		"Mario\\brick.dds",
		"Mario\\bullet.dds",
		"Mario\\coin.dds",
		"Mario\\explode.dds",
		"Mario\\flower.dds",
		"Mario\\mario.dds",
		"Mario\\mushroom.dds",
		"Mario\\tile.dds",
		"Mario\\monster.dds",
		"Mario\\turtle.dds",
		"Mario\\turtle.dds",
	};

	for (uint32_t i = 0U; i < m_Textures.size(); ++i)
	{
		m_Textures[i].Create(s_TexNames[i]);
	}
}

void Engine::LoadMaps()
{
	static const char *const s_MapNames[Map::eMapCount] =
	{
		"level0.dat",
		"level1.dat"
	};

	for (uint32_t i = 0U; i < m_Maps.size(); ++i)
	{
		m_Maps[i].Create(s_MapNames[i]);
	}

	SetMap(&m_Maps[0]);
}

void Engine::Init(HWND hWnd, uint32_t width, uint32_t height)
{
	D3DEngine::Instance().Initialize(hWnd, width, height, true);

	InitD3DResource();

	LoadTextures();

	m_Objects.emplace_back(new Mario());
}

void Engine::InitD3DResource()
{
	const D3D11_INPUT_ELEMENT_DESC VertexLayout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,      0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	m_VertexShader.Create("Mario.hlsl", "VSMain");
	m_PixelShader.Create("Mario.hlsl", "PSMain");
	m_PixelShaderDark.Create("Mario.hlsl", "PSMainDark");
	m_VertexLayout.Create(m_VertexShader.GetBlob(), VertexLayout, _countof(VertexLayout));

	m_VertexBuffer.CreateAsVertexBuffer(sizeof(Vertex) * m_Vertices.size(), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_IndexBuffer.CreateAsIndexBuffer(sizeof(uint32_t) * m_Indices.size(), D3DBuffer::eGpuReadOnly, m_Indices.data());

	m_AlphaBlend.Create(false, false, 0U, true,
		D3DState::eSrcAlpha, D3DState::eInvSrcAlpha, D3DState::eAdd,
		D3DState::eOne, D3DState::eZero, D3DState::eAdd,
		D3DState::eColorAll);

	m_Viewport = { 0.0f, 0.0f, (float)GameApplication::eWidth, (float)GameApplication::eHeight, 0U };
}

void Engine::DrawStaticObjects()
{
	assert(m_CurrentMap);

	Object2D *pMapObject = m_CurrentMap->GetMapObject();

	uint32_t min = m_CurrentMap->Left() / Map::eObjectWidth;
	uint32_t max = min + GameApplication::eWidth / Map::eObjectWidth;
	int32_t deltaLeft = m_CurrentMap->Left() - min * Map::eObjectWidth;

	for (uint32_t i = 0U; i < m_CurrentMap->Height(); ++i)
	{
		uint32_t left = 0U;

		for (uint32_t j = min; j <= max; ++j)
		{
			const char mark = m_CurrentMap->GetStaticMark(i * m_CurrentMap->Width() + j);

			if (mark > 0)
			{
				uint32_t uvX = mark % Map::eObjectCount * Map::eObjectWidth;
				uint32_t uvY = mark / Map::eObjectCount * Map::eObjectHeight + m_CurrentMap->GetUVYStart();

				uint32_t width = Map::eObjectWidth;
				uint32_t height = Map::eObjectHeight;

				if (min == j)
				{
					uvX += deltaLeft;
				}
				else if (max == j)
				{
					width -= (Map::eObjectWidth - deltaLeft);
				}

				pMapObject->UpdateArea(i * Map::eObjectHeight, left, width, height, uvX, uvY);

				DrawObject(*pMapObject);
			}

			if (min == j)
			{
				left += (Map::eObjectWidth - deltaLeft);
			}
			else
			{
				left += Map::eObjectWidth;
			}
		}
	}
}

void Engine::DrawDynamicObjects()
{
	for each (std::shared_ptr<Object2D> object in m_Objects)
	{
		const Object2D::Area &area = object->GetArea();

		int32_t l = area.Left - m_CurrentMap->Left();
		int32_t t = area.Top;

		if (((l + object->ObjectWidth()) < 0)
			|| ((t + object->ObjectHeight()) < 0)
			|| (l > GameApplication::eWidth)
			|| (t > GameApplication::eHeight))
		{
			continue;
		}

		const Object2D::State &state = object->GetState();

		uint32_t left = state.TexIndex * object->ObjectWidth();
		uint32_t top = 0U;
		uint32_t width = left + object->ObjectWidth();
		uint32_t height = top + object->ObjectHeight();

		if (l < 0)
		{
			left += (-l);
			l = 0;
		}
		else if (l + area.ObjectWidth > GameApplication::eWidth)
		{
			width -= l + object->ObjectWidth() - GameApplication::eWidth;
		}

		if (t < 0)
		{
			top += (-t);
			t = 0;
		}
		else if (t + area.ObjectHeight > GameApplication::eHeight)
		{
			height -= t + object->ObjectHeight() - GameApplication::eHeight;
		}

		object->UpdateArea(t, l, width, height, state.TexIndex * object->ObjectWidth(), 0U);

		DrawObject(*object);
	}
}

void Engine::UpdateVertexBuffer(const Object2D &object)
{
	const Object2D::Area &area = object.GetArea();

	float width = (float)area.ObjectWidth / GameApplication::eWidth * 2.0f;
	float height = (float)area.ObjectHeight / GameApplication::eHeight * 2.0f;

	Vec2 lefttop((float)area.Left / GameApplication::eWidth, (float)(GameApplication::eHeight - area.Top) / GameApplication::eHeight);
	lefttop *= 2.0f;
	lefttop -= 1.0f;

	m_Vertices[0].Position = Vec3(lefttop.x, lefttop.y, 0.0f);
	m_Vertices[1].Position = Vec3(lefttop.x + width, lefttop.y, 0.0f);
	m_Vertices[2].Position = Vec3(lefttop.x + width, lefttop.y - height, 0.0f);
	m_Vertices[3].Position = Vec3(lefttop.x, lefttop.y - height, 0.0f);

	uint32_t imageWidth = object.GetImage()->Width();
	uint32_t imageHeight = object.GetImage()->Height();
	float uwidth = (float)area.ObjectWidth / imageWidth;
	float uheight = (float)area.ObjectHeight / imageHeight;

	Vec2 ulefttop((float)area.UVX / imageWidth, (float)area.UVY / imageHeight);
	m_Vertices[0].UV = Vec2(ulefttop.x, ulefttop.y - uheight);
	m_Vertices[1].UV = Vec2(ulefttop.x + uwidth, ulefttop.y - uheight);
	m_Vertices[2].UV = Vec2(ulefttop.x + uwidth, ulefttop.y);
	m_Vertices[3].UV = ulefttop;
}

void Engine::DrawObject(const Object2D &object)
{
	UpdateVertexBuffer(object);

	m_VertexBuffer.Update(m_Vertices.data(), sizeof(Vertex) * m_Vertices.size());

	D3DEngine::Instance().SetVertexBuffer(m_VertexBuffer, sizeof(Vertex), 0U, 0U);
	D3DEngine::Instance().SetIndexBuffer(m_IndexBuffer, eR32_UInt, 0U);
	D3DEngine::Instance().SetInputLayout(m_VertexLayout);

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_CurrentMap->IsDarkMode() ? m_PixelShaderDark : m_PixelShader);

	D3DEngine::Instance().SetShaderResourceView(object.GetImage()->GetShaderResourceView(), 0U, D3DShader::ePixelShader);
	
	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetBlendState(m_AlphaBlend);

	D3DEngine::Instance().DrawIndexed((uint32_t)m_Indices.size(), 0U, 0, eTriangleList);
}

void Engine::Update(float elapseTime, float /*totalTime*/)
{
	for each (std::shared_ptr<Object2D> object in m_Objects)
	{
		object->Update(elapseTime);
	}
}

void Engine::HandleInput(uint32_t msg, WPARAM wParam, LPARAM /*lParam*/)
{
	if (WM_SYSKEYDOWN != msg && WM_KEYDOWN != msg)
	{
		return;
	}

#if 1
	switch (wParam)
	{
	case VK_RIGHT:
		m_CurrentMap->HorizontalScrolling(Map::eObjectHeight);
		break;
	case VK_LEFT:
		m_CurrentMap->HorizontalScrolling(-1 * Map::eObjectHeight);
		break;
	}
#endif
}

void Engine::RenderScene()
{
	if (!m_CurrentMap)
	{
		LoadMaps();
	}

	D3DEngine::Instance().ResetDefaultRenderSurfaces(m_BackColor);
	D3DEngine::Instance().SetViewport(m_Viewport);

	DrawStaticObjects();

	DrawDynamicObjects();

	D3DEngine::Instance().Flush();
}

void Engine::DrawMap()
{
	D3DEngine::Instance().ResetDefaultRenderSurfaces(m_BackColor);
	D3DEngine::Instance().SetViewport(m_Viewport);

	DrawStaticObjects();

	D3DEngine::Instance().Flush();
}

void Engine::Resize(uint32_t width, uint32_t height)
{
	m_Viewport.Width = (float)width;
	m_Viewport.Height = (float)height;

	D3DEngine::Instance().Resize(width, height);
}

void Engine::AddObject(Object2D::eType type, uint32_t left, uint32_t top)
{
	switch (type)
	{
	case Object2D::eMonster:
		m_Objects.emplace_back(new Monster(left, top));
		break;
	case Object2D::eWalkingTurtle:
		m_Objects.emplace_back(new Turtle(Object2D::eWalkingTurtle, left, top));
		break;
	case Object2D::eFlyingTurtle:
		m_Objects.emplace_back(new Turtle(Object2D::eFlyingTurtle, left, top));
		break;
	default:
		assert(!"Unknown object type!");
	}
}
