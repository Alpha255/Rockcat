#include "Runtime/Camera/Camera.h"

void Camera::SetLookAt(const Vector3& Eye, const Vector3& LookAt)
{
	const Vector3 Up(0.0f, 1.0f, 0.0f);

	m_Eye = Eye;
	m_LookAt = LookAt;

	m_View = Matrix::LookAtLH(Eye, LookAt, Up);
	m_InverseView = Matrix::Inverse(m_View);

	Vector4 ZBasis = m_InverseView.Row(2);

	m_Yaw = std::atan2f(ZBasis.x, ZBasis.z);
	m_Pitch = -std::atan2f(ZBasis.y, std::sqrtf(ZBasis.z * ZBasis.z + ZBasis.x * ZBasis.x));

	if (m_Mode == EMode::ModelViewer)
	{
		Matrix Rotate = Matrix::LookAtLH(Eye, LookAt, Up);
		m_QuatCurrent = Quaternion::RotationMatrix(Rotate);

		auto EyeToPoint = LookAt - Eye;
		SetRadius(EyeToPoint.Length());
	}
}

void Camera::OnKeyboardEvent(const KeyboardEvent& Keyboard)
{
	if (Keyboard.State == EKeyState::Down)
	{
		switch (Keyboard.Key)
		{
		case EKeyboardKey::A:
		case EKeyboardKey::Left:
			m_KeyboardMovements.Delta.x += 1.0f;  /// Strafe Left
			break;
		case EKeyboardKey::D:
		case EKeyboardKey::Right:
			m_KeyboardMovements.Delta.x -= 1.0f;  /// Strafe Right
			break;
		case EKeyboardKey::W:
		case EKeyboardKey::Up:
			m_KeyboardMovements.Delta.z += 1.0f;  /// Move Forward
			break;
		case EKeyboardKey::S:
		case EKeyboardKey::Down:
			m_KeyboardMovements.Delta.z -= 1.0f;  /// Move Backward
			break;
		case EKeyboardKey::Q:
			m_KeyboardMovements.Delta.y += 1.0f;  /// Move Up
			break;
		case EKeyboardKey::E:
			m_KeyboardMovements.Delta.y -= 1.0f;  /// Move Down
			break;
		case EKeyboardKey::Home:
			Reset();
			break;
		}
	}
	else
	{
		m_KeyboardMovements.Delta = Vector3();
	}
}

void Camera::UpdateMouseDelta()
{
	if (m_MouseMovements.IsButtonDown(EMouseButton::LButton))
	{
		Vector2 CursorPos = Platform::GetCurrentCursorPosition();

		auto Delta = CursorPos - m_MouseMovements.LastPos;
		m_MouseMovements.LastPos = CursorPos;

		/// Smooth the relative mouse data over a few frames so it isn't 
		/// jerky when moving slowly at low frame rates.
		float32_t PercentNew = 1.0f / m_FrameCountToSmooth;
		float32_t PercentOld = 1.0f - PercentNew;

		m_MouseMovements.PosDelta = m_MouseMovements.PosDelta * PercentOld + Delta * PercentNew;

		m_RotVelocity = m_MouseMovements.PosDelta * m_RotationSpeed;
	}
}

Vector3 Camera::ScaleToScreenPosition(float32_t X, float32_t Y)
{
	float32_t Sx = -(X - m_Center.x) / (m_Radius * m_Center.x);
	float32_t Sy = (Y - m_Center.y) / (m_Radius * m_Center.y);

	float32_t magnitude = Sx * Sx + Sy * Sy;

	if (magnitude > 1.0f)
	{
		float32_t scale = 1.0f / std::sqrtf(magnitude);
		return Vector3(Sx * scale, Sy * scale);
	}

	return Vector3(Sx, Sy, std::sqrtf(1.0f - magnitude));
}

void Camera::OnMouseEvent(const MouseEvent& Mouse)
{
	static uint32_t Counter = 0u;

	if (Mouse.Button != EMouseButton::None)
	{
		m_MouseMovements.SetButton(Mouse.Button, Mouse.State == EKeyState::Down);
		m_MouseMovements.SetButtonDBClicked(Mouse.Button, Mouse.State == EKeyState::DBClick);
	}

	switch (Mouse.Button)
	{
	case EMouseButton::LButton:
		if (Mouse.State != EKeyState::Up)
		{
			m_MouseMovements.LastPos = Platform::GetCurrentCursorPosition();
			m_LastPos = ScaleToScreenPosition(Mouse.Position.x, Mouse.Position.y);
			m_QuatLast = m_QuatCurrent;
		}
		break;
	case EMouseButton::RButton:
		if (Mouse.State != EKeyState::Up)
		{
			m_MouseMovements.LastPos = Platform::GetCurrentCursorPosition();
		}
		break;
	case EMouseButton::MButton:
		if (Mouse.State != EKeyState::Up)
		{
			m_MouseMovements.LastPos = Platform::GetCurrentCursorPosition();
		}
		break;
	}

	m_MouseMovements.WheelMove = Mouse.WheelDelta != 0;
	m_MouseMovements.WheelDelta += Mouse.WheelDelta;

	if (m_MouseMovements.IsButtonDown(EMouseButton::LButton) && Mouse.OnMove)
	{
		Vector3 currentPos = ScaleToScreenPosition(Mouse.Position.x, Mouse.Position.y);
		Vector3 corss = m_LastPos.Cross(currentPos);
		m_QuatCurrent = m_QuatLast * Quaternion(corss.x, corss.y, corss.z, m_LastPos.Dot(currentPos));
	}
}

void Camera::OnWindowResized(uint32_t Width, uint32_t Height)
{
	SetPerspective(m_Fov, static_cast<float32_t>(Width) / Height, m_NearPlane, m_FarPlane);

	m_Center.x = Width / 2.0f;
	m_Center.y = Height / 2.0f;
}

void Camera::Tick(float32_t ElapsedSeconds)
{
	/// Normalize vector so if moving 2 dirs (left & forward), 
	/// the camera doesn't move faster than if moving in 1 dir
	Vector3 Accel = Normalize(m_KeyboardMovements.Delta) * m_MoveSpeed;

	if (Accel.LengthSq() > 0.0f)
	{
		m_DragTimer = m_DragTimerToZero;
		m_VelocityDrag = m_Velocity / m_DragTimer;
		m_Velocity = Accel;
	}
	else
	{
		if (m_DragTimer > 0.0f)
		{
			m_Velocity -= m_VelocityDrag * ElapsedSeconds;
			m_DragTimer -= ElapsedSeconds;
		}
		else
		{
			m_Velocity = Vector3();
		}
	}

	Vector3 PosDelta = m_Velocity * ElapsedSeconds;

	UpdateMouseDelta();

	if (m_Mode == EMode::FirstPerson)
	{
		if (m_MouseMovements.IsButtonDown(EMouseButton::LButton))
		{
			m_Yaw += m_RotVelocity.x;
			m_Pitch += m_RotVelocity.y;

			m_Pitch = std::min(PI_Div2, std::max(-PI_Div2, m_Pitch));
		}

		Matrix Rot = Matrix::RotationRollPitchYaw(m_Pitch, m_Yaw, 0.0f);

		Vector3 WorldUp(0.0f, 1.0f, 0.0f);
		Vector3 WorldHead(0.0f, 0.0f, 1.0f);

		WorldUp.TransformCoord(Rot);
		WorldHead.TransformCoord(Rot);
		PosDelta.TransformCoord(Rot);

		m_Eye += PosDelta;
		if (m_ClipToBoundary)
		{
			m_Eye = m_Eye.Clamp(m_MinBoundary, m_MaxBoundary);
		}

		m_LookAt = m_Eye + WorldHead;

		m_View = Matrix::LookAtLH(m_Eye, m_LookAt, WorldUp);
		m_InverseView = Matrix::Inverse(m_View);
	}
	else if (m_Mode == EMode::ModelViewer)
	{
		if (m_MouseMovements.WheelMove)
		{
			m_Radius -= m_MouseMovements.WheelDelta * m_Radius * 0.1f;
			m_Radius = Clamp(m_Radius, m_MinRadius, m_MaxRadius);
			m_MouseMovements.WheelDelta = 0;
		}

		Matrix rot = Matrix::Inverse(m_QuatCurrent.ToRotationMatrix());

		Vector3 WorldUp(0.0f, 1.0f, 0.0f);
		Vector3 WorldAhead(0.0f, 0.0f, 1.0f);
		WorldUp.TransformCoord(rot);
		WorldAhead.TransformCoord(rot);
		PosDelta.TransformCoord(rot);

		m_LookAt += PosDelta;
		if (m_ClipToBoundary)
		{
			m_LookAt = m_LookAt.Clamp(m_MinBoundary, m_MaxBoundary);
		}

		m_Eye = m_LookAt - WorldAhead * m_Radius;

		m_View = Matrix::LookAtLH(m_Eye, m_LookAt, WorldUp);
		m_InverseView = Matrix::Inverse(m_View);
	}
}

void Camera::FocusToBounds(const AABB& Bounds)
{
	auto Center = Bounds.Center();
	auto Extents = Bounds.Extents();
	auto Expand = Extents * 1.5f;

	auto Eye = Vector3(Center.x, Center.y + Expand.y, Center.z + Expand.z);

	SetLookAt(Center, Eye);
}
