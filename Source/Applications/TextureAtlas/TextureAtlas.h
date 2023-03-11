#include <vector>
#include "Rect.h"
namespace RayTracer
{
	class RAYTRACING_API TextureAtlas
	{
	public:
		enum HeuristicPolicy
		{
			BAF = 0, ///< -BAF: Positions the rectangle into the smallest free rect into which it fits.
			BSSF, ///< -BSSF: Positions the rectangle against the short side of a free rectangle into which it fits the best.
			BLSF, ///< -BLSF: Positions the rectangle against the long side of a free rectangle into which it fits the best.
			CP ///< -CP: Choosest the placement where the rectangle touches other rects as much as possible.
		};

		TextureAtlas(int a_width = 0, int a_height = 0)
			:m_width(a_width), m_height(a_height)
		{
			m_emptyRects.clear();
			m_emptyRects.emplace_back(Rect(m_width, m_height));
		}

		Rect Insert(int a_width, int a_height);
		void Insert(std::vector<Rect>&a_textures, std::vector<Rect>&a_destination);

		HeuristicPolicy GetHeuristicPolicy() const { return m_heuristicPolicy; }
		void SetHeuristicPolicy(HeuristicPolicy a_val) { m_heuristicPolicy = a_val; }
	private:
		Rect FindTheRectangle(const Rect& a_Texture, float& score1, float& score2);
		bool CheckTheSplit(const Rect & a_target, const Rect& a_Source);
		void MergeRect();
	private:
		bool m_AllowReorder = true;
		HeuristicPolicy m_heuristicPolicy = HeuristicPolicy::BSSF;
		const int m_width = 0;
		const int m_height = 0;
		std::vector<Rect> m_emptyRects;

	};
}