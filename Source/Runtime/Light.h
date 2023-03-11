#pragma once

#include "Runtime/SceneNode.h"

enum class ELightType
{
	Directional,
	Point,
	Spot,
	Area,
	Unknown
};

class ILight : public SceneNode
{
public:
	using SceneNode::SceneNode;

	Color GetColor() const
	{
		return m_Color;
	}

	void SetColor(const Color& Value)
	{
		m_Color = Value;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(SceneNode),
			CEREAL_NVP(m_Color)
		);
	}

	virtual ELightType LightType() const = 0;
protected:
	ILight(const char8_t* Name)
		: SceneNode(SceneNode::EType::Light, Name)
	{
	}

	Color m_Color{ Color::White };
};

class DirectionalLight final : public ILight
{
public:
	using ILight::ILight;

	DirectionalLight(const char8_t* Name = "NamelessDirectionalLight")
		: ILight(Name)
	{
	}

	ELightType LightType() const override final
	{
		return ELightType::Directional;
	}

	float32_t Irradiance() const
	{
		return m_Irradiance;
	}

	void SetIrradiance(float32_t Irradiance)
	{
		m_Irradiance = Irradiance;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ILight),
			CEREAL_NVP(m_Irradiance)
		);
	}
protected:
private:
	float32_t m_Irradiance = 1.0f;
};

class PointLight : public ILight
{
public:
	using ILight::ILight;

	PointLight(const char8_t* Name = "NamelessPointLight")
		: ILight(Name)
	{
	}

	ELightType LightType() const override
	{
		return ELightType::Point;
	}

	float32_t Intensity() const
	{
		return m_Intensity;
	}

	void SetIntensity(float32_t Intensity)
	{
		m_Intensity = Intensity;
	}

	float32_t Radius() const
	{
		return m_Radius;
	}

	void SetRadius(float32_t Radius)
	{
		m_Radius = Radius;
	}

	float32_t Range() const
	{
		return m_Range;
	}

	void SetRange(float32_t Range)
	{
		m_Range = Range;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ILight),
			CEREAL_NVP(m_Intensity),
			CEREAL_NVP(m_Radius),
			CEREAL_NVP(m_Range)
		);
	}
protected:
private:
	float32_t m_Intensity = 1.0f;
	float32_t m_Radius = 0.0f;
	float32_t m_Range = 0.0f;
};

class SpotLight final : public PointLight
{
public:
	using PointLight::PointLight;

	SpotLight(const char8_t* Name = "NamelessSpotLight")
		: PointLight(Name)
	{
	}

	ELightType LightType() const override final
	{
		return ELightType::Spot;
	}

	float32_t InnerAngle() const
	{
		return m_InnerAngle;
	}

	void SetInnerAngle(float32_t InnerAngle)
	{
		m_InnerAngle = InnerAngle;
	}

	float32_t OuterAngle() const
	{
		return m_OuterAngle;
	}

	void SetOuterAngle(float32_t OuterAngle)
	{
		m_OuterAngle = OuterAngle;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ILight),
			CEREAL_NVP(m_InnerAngle),
			CEREAL_NVP(m_OuterAngle)
		);
	}
protected:
private:
	float32_t m_InnerAngle = 180.0f;
	float32_t m_OuterAngle = 180.0f;
};

class AreaLight final : public ILight
{
public:
	using ILight::ILight;

	AreaLight(const char8_t* Name = "NamelessAreaLight")
		: ILight(Name)
	{
	}

	ELightType LightType() const override final
	{
		return ELightType::Area;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ILight)
		);
	}
};
