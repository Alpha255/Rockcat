#pragma once

#include "Core/Math/Color.h"
#include "Asset/Asset.h"
#include "Asset/RenderResource.h"
#include "RHI/RHITexture.h"

class Texture : public Asset, public RenderResource
{
public:
	using Asset::Asset;

	bool IsLinear() const { return m_Linear; }

	const RHITexture* GetRHI() const { assert(m_Texture); return m_Texture.get(); }

	//template<class Archive>
	//void serialize(Archive& Ar)
	//{
	//	Ar(
	//		CEREAL_BASE(Asset)/*,
	//		CEREAL_NVP(m_Path)*/
	//	);
	//}
protected:
	friend class AssimpSceneLoader;
	friend class StbTextureLoader;
	friend class DDSTextureLoader;

	static Texture* CreateColoredTexture(std::string_view Name, const Math::Color& Value, uint32_t Width = 1u, uint32_t Height = 1u);

	void CreateRHI(class RHIDevice& Device, const RHITextureDesc& Desc);

	void SetLinear(bool Linear) { m_Linear = Linear; }

	bool m_Linear = false;
	RHITexturePtr m_Texture;
};
