#include <algorithm>
#include "TextureAtlas.h"
namespace RayTracer
{
	Rect TextureAtlas::Insert(int a_width, int a_height)
	{
		
		float score1 = std::numeric_limits<float>::max();
		float score2 = std::numeric_limits<float>::max();
		
		Rect result = FindTheRectangle(Rect(a_width, a_height), score1, score2);


		if (result.m_height == 0.0f)
			return result;

		for (auto i = 0; i < m_emptyRects.size(); ++i)
		{
			if (CheckTheSplit(result, m_emptyRects[i]))
			{
				m_emptyRects.erase(m_emptyRects.begin() + i);
				--i;
			}
		}

		MergeRect();

		return result;
	}

	void TextureAtlas::Insert(std::vector<Rect>& a_textures, std::vector<Rect>& a_destination)
	{
		//  Maximal Rectangles Algorithms
		if (m_AllowReorder)
		{
			//std::sort(a_textures.begin(), a_textures.end(), nullptr);
		}
		while (!a_textures.empty())
		{
			float bestScore1 = std::numeric_limits<float>::max();
			float bestScore2 = std::numeric_limits<float>::max();
			int index = -1;
			Rect bestChoosen;
			for (int ind = 0;ind < a_textures.size();++ind)
			{
				Rect texture = a_textures[ind];
				if (texture.m_width == 0 || texture.m_height == 0)
					continue;
				float score1, score2 = 0.0f;
				Rect tempRect = FindTheRectangle(texture, score1, score2);
				if (bestScore1 > score1 || (bestScore1 == score1&&bestScore2 > score2))
				{
					bestScore1 = score1;
					bestScore2 = score2;
					index = ind;
					bestChoosen = tempRect;
					bestChoosen.m_width = texture.m_width;
					bestChoosen.m_height = texture.m_height;
				}
			}
			if (index == -1)
				break;

			for (auto i = 0; i < m_emptyRects.size(); ++i)
			{
				if (CheckTheSplit(bestChoosen, m_emptyRects[i]))
				{
					m_emptyRects.erase(m_emptyRects.begin() + i);
					--i;
				}
			}
			MergeRect();
			Rect result(bestChoosen.m_width, bestChoosen.m_width, bestChoosen.m_x, bestChoosen.m_y);
			//result.m_name = a_textures[index].m_name;
			a_destination.emplace_back(result);
			a_textures.erase(a_textures.begin() + index);
		}
	}
	Rect TextureAtlas::FindTheRectangle(const Rect & a_Texture, float & score1, float & score2)
	{
		Rect bestChoosen;
		score1 = std::numeric_limits<float>::max();
		score2 = std::numeric_limits<float>::max();
		if (m_heuristicPolicy == HeuristicPolicy::CP)
		{
			score1 = score2 = 0;
		}
		for (const Rect& emptyRect : m_emptyRects)
		{
			switch (m_heuristicPolicy)
			{
			case BAF:
				/// score1:area
				/// score2:bestshort
				if (emptyRect.m_width >= a_Texture.m_width&& emptyRect.m_height >= a_Texture.m_height)
				{
					if (score1 > (emptyRect.m_width*emptyRect.m_height - a_Texture.m_width*a_Texture.m_height)
						|| ((score1 == emptyRect.m_width*emptyRect.m_height - a_Texture.m_width*a_Texture.m_height)
							&& score2 > std::min(emptyRect.m_width - a_Texture.m_width, emptyRect.m_height - a_Texture.m_height)))
					{

						score1 = emptyRect.m_width*emptyRect.m_height - a_Texture.m_width*a_Texture.m_height;
						bestChoosen = emptyRect;
						score2 = std::min(emptyRect.m_width - a_Texture.m_width, emptyRect.m_height - a_Texture.m_height);
					}
				}
				break;
			case BSSF:
				/// score1:best short side
				/// score2:best long side
				if (emptyRect.m_width >= a_Texture.m_width&& emptyRect.m_height >= a_Texture.m_height)
				{
					if (score1 > std::min(emptyRect.m_width - a_Texture.m_width, emptyRect.m_height - a_Texture.m_height)
						|| ((score1 == std::min(emptyRect.m_width - a_Texture.m_width, emptyRect.m_height - a_Texture.m_height))
							&& score2 > std::max(emptyRect.m_width - a_Texture.m_width, emptyRect.m_height - a_Texture.m_height)))
					{
						score1 = std::min(emptyRect.m_width - a_Texture.m_width, emptyRect.m_height - a_Texture.m_height);
						bestChoosen = emptyRect;
						score2 = std::max(emptyRect.m_width - a_Texture.m_width, emptyRect.m_height - a_Texture.m_height);
					}
				}
				break;
			case BLSF:
				/// score1:best short side
				/// score2:best long side
				if (emptyRect.m_width >= a_Texture.m_width&& emptyRect.m_height >= a_Texture.m_height)
				{
					if (score2 > std::max(emptyRect.m_width - a_Texture.m_width, emptyRect.m_height - a_Texture.m_height)
						|| (score2 == std::max(emptyRect.m_width - a_Texture.m_width, emptyRect.m_height - a_Texture.m_height)
							&& score1 > std::min(emptyRect.m_width - a_Texture.m_width, emptyRect.m_height - a_Texture.m_height)))
					{
						score1 = std::min(emptyRect.m_width - a_Texture.m_width, emptyRect.m_height - a_Texture.m_height);
						bestChoosen = emptyRect;
						score2 = std::max(emptyRect.m_width - a_Texture.m_width, emptyRect.m_height - a_Texture.m_height);
					}
				}
				break;
			case CP:

				break;
			default:
				break;
			}

		}
		return bestChoosen;
	}
	bool TextureAtlas::CheckTheSplit(const Rect & a_target, const Rect& a_Source)
	{
		if (a_target.m_x >= a_Source.m_x + a_Source.m_width || a_target.m_y >= a_Source.m_y + a_Source.m_height
			|| a_target.m_x + a_target.m_width <= a_Source.m_x || a_target.m_y + a_target.m_height <= a_Source.m_y)
		{
			return false;
		}
		if (a_target.m_x < a_Source.m_x + a_Source.m_width&&a_target.m_x + a_target.m_width>a_Source.m_x)
		{
			if (a_target.m_y > a_Source.m_y)
			{
				m_emptyRects.emplace_back(Rect(a_Source.m_width, a_target.m_y - a_Source.m_y,
					a_Source.m_x, a_Source.m_y));
			}
			if (a_target.m_y + a_target.m_height < a_Source.m_y + a_Source.m_height)
			{
				m_emptyRects.emplace_back(Rect(a_Source.m_width, a_Source.m_y + a_Source.m_height - a_target.m_y - a_target.m_height,
					a_Source.m_x, a_target.m_y + a_target.m_height));
			}
		}

		if (a_target.m_y < a_Source.m_y + a_Source.m_height&& a_target.m_y + a_target.m_height > a_Source.m_y)
		{
			if (a_target.m_x > a_Source.m_x)
			{
				m_emptyRects.emplace_back(Rect(a_target.m_x - a_Source.m_x, a_Source.m_height, a_Source.m_x, a_Source.m_y));
			}

			if (a_target.m_x + a_target.m_width < a_Source.m_x + a_Source.m_width)
			{
				m_emptyRects.emplace_back(Rect(a_Source.m_x + a_Source.m_width - a_target.m_x - a_target.m_width, a_Source.m_height,
					a_target.m_x + a_target.m_width, a_Source.m_y));
			}
		}
		return true;
	}
	void TextureAtlas::MergeRect()
	{
		for (auto t = 0; t < m_emptyRects.size(); ++t)
			for (auto j = t + 1; j < m_emptyRects.size(); ++j)
			{
				if (m_emptyRects[j].IsContain(m_emptyRects[t]))
				{
					m_emptyRects.erase(m_emptyRects.begin() + t);
					--t;
					break;
				}
				if (m_emptyRects[t].IsContain(m_emptyRects[j]))
				{
					m_emptyRects.erase(m_emptyRects.begin() + j);
					--j;
				}
			}
	}

}