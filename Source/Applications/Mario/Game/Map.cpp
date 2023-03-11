#include "Map.h"
#include "System.h"
#include "Image.h"
#include "GameApplication.h"
#include "Engine.h"
#include "ResourceFile.h"

#include <string>
#include <fstream>

void Map::Create(const char *pFileName)
{
	assert(pFileName);

	ResourceFile map(pFileName);
	std::ifstream mapFile(map.GetFilePath(), std::ios::in);

	assert(mapFile.good());

	mapFile >> m_DarkBg;

	mapFile >> m_Width >> m_Height;
	assert(m_Width && m_Height);
	size_t size = m_Width * m_Height;
	char *pStaticMarks = new char[size]();
	char *pDynamicMarks = new char[size]();
	assert(pStaticMarks && pDynamicMarks);

	/// Static objects, such cloud, hill, etc.
	char *pObject = pStaticMarks;
	int content = 0;
	for (uint32_t i = 0U; i < m_Height; ++i)
	{
		for (uint32_t j = 0U; j < m_Width; ++j)
		{
			mapFile >> content;
			*pObject++ = (char)content - 1;
		}
	}

	/// Dynamic objects, such as coin mark, brick, tube, etc.
	pObject = pDynamicMarks;
	for (uint32_t i = 0U; i < m_Height; ++i)
	{
		for (uint32_t j = 0U; j < m_Width; ++j)
		{
			mapFile >> content;
			*pObject++ = (char)content - 1;
		}
	}

	/// NPCs
	RegisterNPCs(mapFile);

	mapFile.close();

	m_StaticMarks.reset(pStaticMarks);
	m_DynamicMarks.reset(pDynamicMarks);
	m_ImageObject = Object2D(Object2D::eTile, 32U, 32U);

	m_UVYStart = m_ImageObject.GetImage()->Height() - Map::eObjectHeight;

	m_IsValid = true;
}

void Map::RegisterNPCs(std::ifstream &mapFile)
{
	uint32_t npcCount = 0U;
	uint32_t left = 0, top = 0;

	for (uint32_t i = Object2D::eMonster; i < Object2D::eTypeCount; ++i)
	{
		/// Monsters
		mapFile >> npcCount;
		for (uint32_t j = 0U; j < npcCount; ++j)
		{
			mapFile >> left;
			mapFile >> top;
			Engine::Instance().AddObject((Object2D::eType)i, left, top);
		}
	}
}

void Map::HorizontalScrolling(int32_t delta)
{
	int32_t mapWidth = m_Width * (GameApplication::eWidth / eObjectWidth + 8.8);
	m_Left += delta;
	m_Left = m_Left < 0 ? 0 : m_Left;
	m_Left = m_Left >= mapWidth ? mapWidth : m_Left;
}
