#pragma once

#include "Core/Math/Color.h"

class Light
{
public:
	enum class ELightType
	{
		Directional,
		Point,
		Spot,
		Area
	};

	Light() = default;
	Light(const char* Name)
		: m_Color(Math::Color::White)
		, m_Name(Name)
	{
	}

	const Math::Color& GetColor() const { return m_Color; }
	void SetColor(const Math::Color& Value) { m_Color = Value; }

	const char* GetName() const { return m_Name.c_str(); }
	void SetName(const char* Name) { m_Name = Name; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Color),
			CEREAL_NVP(m_Name)
		);
	}

	virtual ELightType GetType() const = 0;
protected:
	Math::Color m_Color;
	std::string m_Name;
};

class DirectionalLight final : public Light
{
public:
	using Light::Light;

	ELightType GetType() const override final { return ELightType::Directional; }

	float GetIrradiance() const { return m_Irradiance; }
	void SetIrradiance(float Irradiance) { m_Irradiance = Irradiance; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(Light),
			CEREAL_NVP(m_Irradiance)
		);
	}
protected:
private:
	float m_Irradiance = 1.0f;
};

class PointLight : public Light
{
public:
	using Light::Light;

	ELightType GetType() const override { return ELightType::Point; }

	float GetIntensity() const { return m_Intensity; }
	void SetIntensity(float Intensity) { m_Intensity = Intensity; }

	float GetRadius() const { return m_Radius; }
	void SetRadius(float Radius) { m_Radius = Radius; }

	float GetRange() const { return m_Range; }
	void SetRange(float Range) { m_Range = Range; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(Light),
			CEREAL_NVP(m_Intensity),
			CEREAL_NVP(m_Radius),
			CEREAL_NVP(m_Range)
		);
	}
protected:
private:
	float m_Intensity = 1.0f;
	float m_Radius = 0.0f;
	float m_Range = 0.0f;
};

class SpotLight final : public PointLight
{
public:
	using PointLight::PointLight;

	ELightType GetType() const override final { return ELightType::Spot; }

	float GetInnerAngle() const { return m_InnerAngle; }
	void SetInnerAngle(float InnerAngle) { m_InnerAngle = InnerAngle; }

	float GetOuterAngle() const { return m_OuterAngle; }
	void SetOuterAngle(float OuterAngle) { m_OuterAngle = OuterAngle; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(Light),
			CEREAL_NVP(m_InnerAngle),
			CEREAL_NVP(m_OuterAngle)
		);
	}
protected:
private:
	float m_InnerAngle = 180.0f;
	float m_OuterAngle = 180.0f;
};

class AreaLight final : public Light
{
public:
	using Light::Light;

	ELightType GetType() const override final { return ELightType::Area; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(Light)
		);
	}
};
