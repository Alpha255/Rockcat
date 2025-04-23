#pragma once

#include "Core/Math/Matrix.h"
#include "Core/Tickable.h"
#include "MessageRouter.h"

class Camera : public MessageHandler, public ITickable
{
public:
	enum class EMode : uint8_t
	{
		FirstPerson,
		ModelViewer
	};

	Camera(EMode Mode)
		: Camera(Mode, Math::Vector3(0.0f, 0.0f, 0.0f), Math::Vector3(0.0f, 0.0f, 1.0f), Math::PI_Div4, 1.0, 1.0f, 1000.0f)
	{
	}

	Camera(EMode Mode, const Math::Vector3& Eye, const Math::Vector3& LookAt, float FOV, float Aspect, float NearPlane, float FarPlane)
		: m_DefaultEye(Eye)
		, m_DefaultLookAt(LookAt)
		, m_Mode(Mode)
	{
		SetLookAt(Eye, LookAt);

		SetPerspective(FOV, Aspect, NearPlane, FarPlane);
	}

	virtual ~Camera() = default;

	inline void SetMode(EMode Mode) { m_Mode = Mode; }
	inline const Math::Matrix& GetWorldMatrix() const { return m_World; }
	inline const Math::Matrix& GetViewMatrix() const { return m_View; }
	inline const Math::Matrix& GetProjectionMatrix() const { return m_Projection; }
	inline const Math::Vector3& GetEyePosition() const { return m_Eye; }
	inline const Math::Vector3& GetLookAt() const { return m_LookAt; }
	inline Math::Matrix GetViewProjectionMatrix() const { return m_View * m_Projection; }

	inline void SetClipToBoundary(bool Clip, const Math::Vector3& Min, const Math::Vector3& Max)
	{
		m_ClipToBoundary = Clip;
		m_MinBoundary = Min;
		m_MaxBoundary = Max;
	}

	inline void SetSpeed(float RotationSpeed = 0.01f, float MoveSpeed = 5.0f)
	{
		m_RotationSpeed = RotationSpeed;
		m_MoveSpeed = MoveSpeed;
	}

	inline void SetPerspective(float FOV, float Aspect, float NearPlane, float FarPlane)
	{
		m_Fov = FOV;
		m_Aspect = Aspect;
		m_NearPlane = NearPlane;
		m_FarPlane = FarPlane;

		m_Projection = Math::Matrix::PerspectiveFovLH(FOV, Aspect, NearPlane, FarPlane);
	}

	inline void Reset()
	{
		SetLookAt(m_DefaultEye, m_DefaultLookAt);

		if (m_Mode == EMode::ModelViewer)
		{
			m_QuatCurrent.Identity();
			m_QuatLast.Identity();
		}
	}

	void SetRadius(float Radius) { assert(m_Mode == EMode::ModelViewer); m_Radius = Radius; }

	void SetLookAt(const Math::Vector3& Eye, const Math::Vector3& LookAt);

	void OnMouseEvent(const MouseEvent& Mouse) override;

	void OnKeyboardEvent(const KeyboardEvent& Keyboard) override;

	void OnWindowResized(uint32_t Width, uint32_t Height) override;

	void Tick(float ElapsedSeconds) override final;

	void FocusToBounds(const Math::AABB& Bounds);

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Eye),
			CEREAL_NVP(m_LookAt),
			CEREAL_NVP(m_DefaultEye),
			CEREAL_NVP(m_DefaultLookAt),
			CEREAL_NVP(m_MinBoundary),
			CEREAL_NVP(m_MaxBoundary),
			CEREAL_NVP(m_ModelCenter),
			CEREAL_NVP(m_Fov),
			CEREAL_NVP(m_Aspect),
			CEREAL_NVP(m_NearPlane),
			CEREAL_NVP(m_FarPlane),
			CEREAL_NVP(m_RotationSpeed),
			CEREAL_NVP(m_MoveSpeed),
			CEREAL_NVP(m_Yaw),
			CEREAL_NVP(m_Pitch),
			CEREAL_NVP(m_Mode),
			CEREAL_NVP(m_ClipToBoundary)
		);
	}
protected:
	friend class cereal::access;

	Camera() = default;

	enum EMovement
	{
		StrafeLeft,
		StrafeRight,
		Forward,
		Backward,
		Up,
		Down,
		Max
	};

	struct KeyboardMovements
	{
		Math::Vector3 Delta;
	};

	struct MouseMovements
	{
		int32_t WheelDelta = 0;
		Math::Vector2 PosDelta;
		Math::Vector2 LastPos;
		bool WheelMove = false;

		std::array<std::pair<bool, bool>, 3ull> Buttons;

		inline void SetButton(EMouseButton Button, bool ButtonDown) { Buttons[static_cast<uint32_t>(Button)].first = ButtonDown; }
		inline const bool IsButtonDown(EMouseButton Button) const { return Buttons[static_cast<uint32_t>(Button)].first; }
		inline void SetButtonDoubleClicked(EMouseButton Button, bool Clicked) { Buttons[static_cast<uint32_t>(Button)].second = Clicked; }
		inline const bool IsButtonDoubleClicked(EMouseButton Button) const { return Buttons[static_cast<uint32_t>(Button)].second; }
	};

	void UpdateMouseDelta();

	Math::Vector3 ScaleToScreenPosition(float X, float Y);

	Math::Matrix m_World;
	Math::Matrix m_View;
	Math::Matrix m_InverseView;
	Math::Matrix m_Projection;
	Math::Vector3 m_Eye;
	Math::Vector3 m_LookAt;
	Math::Vector3 m_DefaultEye;
	Math::Vector3 m_DefaultLookAt;
	Math::Vector3 m_MinBoundary;
	Math::Vector3 m_MaxBoundary;
	Math::Vector2 m_RotVelocity;
	Math::Vector3 m_Velocity;
	Math::Vector3 m_VelocityDrag;
	Math::Vector3 m_ModelCenter;
	Math::Vector2 m_Center;
	Math::Vector3 m_LastPos;

	float m_Fov = 0.0f;
	float m_Aspect = 1.0f;
	float m_NearPlane = 0.0f;
	float m_FarPlane = 0.0f;
	float m_RotationSpeed = 0.01f;
	float m_MoveSpeed = 5.0f;
	float m_Yaw = 0.0f;
	float m_Pitch = 0.0f;
	uint32_t m_FrameCountToSmooth = 2u;
	float m_DragTimerToZero = 0.25f;
	float m_DragTimer = 0.0f;

	float m_Radius = 5.0f;
	float m_DefaultRadius = 5.0f;
	float m_MinRadius = 1.0f;
	float m_MaxRadius = std::numeric_limits<float>::max();

	Math::Quaternion m_QuatLast;
	Math::Quaternion m_QuatCurrent;

	EMode m_Mode = EMode::FirstPerson;
	bool m_ClipToBoundary = false;
	
	KeyboardMovements m_KeyboardMovements;
	MouseMovements m_MouseMovements;
};
