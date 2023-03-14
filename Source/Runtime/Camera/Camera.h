#pragma once

#include "Runtime/Scene/SceneNode.h"

class Camera : public SceneNode, public IInputHandler
{
public:
	enum class EMode : uint8_t
	{
		FirstPerson,
		ModelViewer
	};

	Camera(EMode Mode)
		: Camera(Mode, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Math::PI_Div4, 1.0, 1.0f, 1000.0f)
	{
	}

	Camera(EMode Mode, const Vector3& Eye, const Vector3& LookAt, float32_t FOV, float32_t Aspect, float32_t NearPlane, float32_t FarPlane)
		: SceneNode(SceneNode::EType::Camera, "$NamelessCamera")
		, m_DefaultEye(Eye)
		, m_DefaultLookAt(LookAt)
		, m_Mode(Mode)
	{
		SetLookAt(Eye, LookAt);

		SetPerspective(FOV, Aspect, NearPlane, FarPlane);
	}

	virtual ~Camera() = default;

	inline void SetMode(EMode Mode)
	{
		m_Mode = Mode;
	}

	inline const Matrix& WorldMatrix() const
	{
		return m_World;
	}

	inline const Matrix& ViewMatrix() const
	{
		return m_View;
	}

	inline const Matrix& ProjectionMatrix() const
	{
		return m_Projection;
	}

	inline const Vector3& Eye() const
	{
		return m_Eye;
	}

	inline const Vector3& LookAt() const
	{
		return m_LookAt;
	}

	inline Matrix ViewProjectionMatrix() const
	{
		return m_View * m_Projection;
	}

	inline void SetClipToBoundary(bool8_t Clip, const Vector3& Min, const Vector3& Max)
	{
		m_ClipToBoundary = Clip;
		m_MinBoundary = Min;
		m_MaxBoundary = Max;
	}

	inline void SetSpeed(float32_t RotationSpeed = 0.01f, float32_t MoveSpeed = 5.0f)
	{
		m_RotationSpeed = RotationSpeed;
		m_MoveSpeed = MoveSpeed;
	}

	inline void SetPerspective(float32_t FOV, float32_t Aspect, float32_t NearPlane, float32_t FarPlane)
	{
		m_Fov = FOV;
		m_Aspect = Aspect;
		m_NearPlane = NearPlane;
		m_FarPlane = FarPlane;

		m_Projection = Matrix::PerspectiveFovLH(FOV, Aspect, NearPlane, FarPlane);
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

	void SetRadius(float32_t Radius)
	{
		assert(m_Mode == EMode::ModelViewer);
		m_Radius = Radius;
	}

	void SetLookAt(const Vector3& Eye, const Vector3& LookAt);

	void OnMouseEvent(const MouseEvent& Mouse) override;

	void OnKeyboardEvent(const KeyboardEvent& Keyboard) override;

	void OnWindowResized(uint32_t Width, uint32_t Height) override;

	void Tick(float32_t ElapsedSeconds) override final;

	void FocusToBounds(const AABB& Bounds);

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(SceneNode),
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
		Vector3 Delta;
	};

	struct MouseMovements
	{
		int32_t WheelDelta = 0;
		Vector2 PosDelta;
		Vector2 LastPos;
		bool8_t WheelMove = false;

		std::array<std::pair<bool8_t, bool8_t>, 3ull> Buttons;

		inline void SetButton(EMouseButton Button, bool8_t ButtonDown)
		{
			Buttons[static_cast<uint32_t>(Button)].first = ButtonDown;
		}

		inline const bool8_t IsButtonDown(EMouseButton Button) const
		{
			return Buttons[static_cast<uint32_t>(Button)].first;
		}

		inline void SetButtonDBClicked(EMouseButton Button, bool8_t Clicked)
		{
			Buttons[static_cast<uint32_t>(Button)].second = Clicked;
		}

		inline const bool8_t IsButtonDBClicked(EMouseButton Button) const
		{
			return Buttons[static_cast<uint32_t>(Button)].second;
		}
	};

	void UpdateMouseDelta();

	Vector3 ScaleToScreenPosition(float32_t X, float32_t Y);

	Matrix m_World;
	Matrix m_View;
	Matrix m_InverseView;
	Matrix m_Projection;
	Vector3 m_Eye;
	Vector3 m_LookAt;
	Vector3 m_DefaultEye;
	Vector3 m_DefaultLookAt;
	Vector3 m_MinBoundary;
	Vector3 m_MaxBoundary;
	Vector2 m_RotVelocity;
	Vector3 m_Velocity;
	Vector3 m_VelocityDrag;
	Vector3 m_ModelCenter;
	Vector2 m_Center;
	Vector3 m_LastPos;

	float32_t m_Fov = 0.0f;
	float32_t m_Aspect = 1.0f;
	float32_t m_NearPlane = 0.0f;
	float32_t m_FarPlane = 0.0f;
	float32_t m_RotationSpeed = 0.01f;
	float32_t m_MoveSpeed = 5.0f;
	float32_t m_Yaw = 0.0f;
	float32_t m_Pitch = 0.0f;
	uint32_t m_FrameCountToSmooth = 2u;
	float32_t m_DragTimerToZero = 0.25f;
	float32_t m_DragTimer = 0.0f;

	float32_t m_Radius = 5.0f;
	float32_t m_DefaultRadius = 5.0f;
	float32_t m_MinRadius = 1.0f;
	float32_t m_MaxRadius = std::numeric_limits<float32_t>().max();

	Quaternion m_QuatLast;
	Quaternion m_QuatCurrent;

	EMode m_Mode = EMode::FirstPerson;
	bool8_t m_ClipToBoundary = false;
	
	KeyboardMovements m_KeyboardMovements;
	MouseMovements m_MouseMovements;
};
