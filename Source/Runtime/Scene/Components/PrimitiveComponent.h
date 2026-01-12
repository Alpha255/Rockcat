#pragma once

#include "Scene/Components/ComponentBase.h"
#include "Scene/BoxSphereBounds.h"

///  PrimitiveBuffers ???
class PrimitiveComponent : public ComponentBase
{
public:
	REGISTER_COMPONENT_ID(PrimitiveComponent);

	using ComponentBase::ComponentBase;

	inline const BoxSphereBounds& GetBounds() const { return m_Bounds; }
	inline void SetBounds(const BoxSphereBounds& Bounds) { m_Bounds = Bounds; }

	inline const BoxSphereBounds& GetLocalBounds() const { return m_LocalBounds; }
	inline void SetLocalBounds(const BoxSphereBounds& Bounds) { m_LocalBounds = Bounds; }

	inline bool IsCastShadow() const { return m_CastShadow; }
	inline void SetCastShadow(bool CastShadow) { m_CastShadow = CastShadow; }

	inline const FName& GetName() const { return m_Name; }
	inline void SetName(FName&& Name) { m_Name = std::move(Name); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ComponentBase),
			CEREAL_NVP(m_Bounds),
			CEREAL_NVP(m_LocalBounds),
			CEREAL_NVP(m_CastShadow),
			CEREAL_NVP(m_Name)
		);
	}
private:
	BoxSphereBounds m_Bounds;
	BoxSphereBounds m_LocalBounds;

	bool m_CastShadow = true;

	FName m_Name;
};