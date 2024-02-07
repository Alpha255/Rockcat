#pragma once

#include "Core/Math/Matrix.h"

NAMESPACE_START(Math)

class Quaternion : public Float4
{
public:
	/// The DirectXMath quaternion functions use an XMVECTOR 4-vector to represent quaternions, 
	/// where the X, Y, and Z components are the vector part and the W component is the scalar part.
	Quaternion(float X, float Y, float Z, float W)
		: Float4(X, Y, Z, W)
	{
	}

	Quaternion()
		: Quaternion(0.0f, 0.0f, 0.0f, 1.0f)
	{
	}

	inline void Identity()
	{
		VECTOR_STORE(4, this, DirectX::XMQuaternionIdentity());
	}

	inline void Inverse()
	{
		VECTOR_STORE(4, this, DirectX::XMQuaternionInverse(VECTOR_LOAD(4, this)));
	}

	inline void Normalize()
	{
		VECTOR_STORE(4, this, DirectX::XMQuaternionNormalize(VECTOR_LOAD(4, this)));
	}

	inline void NormalizeEst()
	{
		VECTOR_STORE(4, this, DirectX::XMQuaternionNormalizeEst(VECTOR_LOAD(4, this)));
	}

	inline Quaternion Conjugate() const
	{
		Quaternion Ret(0.0f, 0.0f, 0.0f, 0.0f);
		VECTOR_STORE(4, &Ret, DirectX::XMQuaternionConjugate(VECTOR_LOAD(4, this)));
		return Ret;
	}

	inline Vector4 LogN() const
	{
		return Vector4();
	}

	inline Vector4 Exp() const
	{
		return Vector4();
	}

	inline Quaternion operator+(const Quaternion& Other) const
	{
		Quaternion Ret;
		VECTOR_STORE(4, &Ret, DirectX::XMVectorAdd(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &Other)));
		return Ret;
	}

	inline Quaternion& operator+=(const Quaternion& Other)
	{
		VECTOR_STORE(4, this, DirectX::XMVectorAdd(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &Other)));
		return *this;
	}

	inline Quaternion operator-(const Quaternion& Other) const
	{
		Quaternion Ret;
		VECTOR_STORE(4, &Ret, DirectX::XMVectorSubtract(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &Other)));
		return Ret;
	}

	inline Quaternion& operator-=(const Quaternion& Other)
	{
		VECTOR_STORE(4, this, DirectX::XMVectorSubtract(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &Other)));
		return *this;
	}

	inline Quaternion operator*(const Quaternion& Other) const
	{
		Quaternion Ret;
		VECTOR_STORE(4, &Ret, DirectX::XMQuaternionMultiply(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &Other)));
		return Ret;
	}

	inline Quaternion& operator*=(const Quaternion& Other)
	{
		VECTOR_STORE(4, this, DirectX::XMQuaternionMultiply(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &Other)));
		return *this;
	}

	inline bool IsNaN() const
	{
		return DirectX::XMQuaternionIsNaN(VECTOR_LOAD(4, this));
	}

	inline bool IsInfinite() const
	{
		return DirectX::XMQuaternionIsInfinite(VECTOR_LOAD(4, this));
	}

	inline bool IsIdentity() const
	{
		return DirectX::XMQuaternionIsIdentity(VECTOR_LOAD(4, this));
	}

	inline bool operator==(const Quaternion& Other) const
	{
		return DirectX::XMQuaternionEqual(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &Other));
	}

	inline bool operator!=(const Quaternion& Other) const
	{
		return DirectX::XMQuaternionNotEqual(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &Other));
	}

	inline float Dot(const Quaternion& Other) const
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionDot(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &Other)));
	}

	inline float Length() const
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionLength(VECTOR_LOAD(4, this)));
	}

	inline float LengthSq() const
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionLengthSq(VECTOR_LOAD(4, this)));
	}

	inline float ReciprocalLength() const
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionReciprocalLength(VECTOR_LOAD(4, this)));
	}

	inline void RotationRollPitchYaw(float Pitch, float Yaw, float Roll)
	{
		VECTOR_STORE(4, this, DirectX::XMQuaternionRotationRollPitchYaw(Pitch, Yaw, Roll));
	}

	inline void RotationRollPitchYaw(const Vector3& PitchYawRoll)
	{
		VECTOR_STORE(4, this, DirectX::XMQuaternionRotationRollPitchYaw(PitchYawRoll.x, PitchYawRoll.y, PitchYawRoll.z));
	}

	inline void RotationAxis(const Vector3& Axis, float Angle)
	{
		VECTOR_STORE(4, this, DirectX::XMQuaternionRotationAxis(VECTOR_LOAD(3, &Axis), Angle));
	}

	inline void RotationXAxis(float Angle)
	{
		static Vector3 s_XAxis(1.0f, 0.0f, 0.0f);
		RotationAxis(s_XAxis, Angle);
	}

	inline void RotationYAxis(float Angle)
	{
		static Vector3 s_YAxis(0.0f, 1.0f, 0.0f);
		RotationAxis(s_YAxis, Angle);
	}

	inline void RotationZAxis(float Angle)
	{
		static Vector3 s_ZAxis(0.0f, 0.0f, 1.0f);
		RotationAxis(s_ZAxis, Angle);
	}

	inline void RotationMatrix(const Matrix& Matrix)
	{
		VECTOR_STORE(4, this, DirectX::XMQuaternionRotationMatrix(MATRIX_LOAD(&Matrix)));
	}

	inline Matrix GetRotationMatrix()
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixRotationQuaternion(VECTOR_LOAD(4, this)));
		return Ret;
	}

	inline void GetAxisAngle(Vector3& Axis, float& Angle)
	{
		DirectX::XMVECTOR AxisV;
		DirectX::XMQuaternionToAxisAngle(&AxisV, &Angle, VECTOR_LOAD(4, this));
		VECTOR_STORE(3, &Axis, AxisV);
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(x),
			CEREAL_NVP(y),
			CEREAL_NVP(z),
			CEREAL_NVP(w)
		);
	}
protected:
private:
};

inline Quaternion Slerp(const Quaternion& Q0, const Quaternion& Q1, float Factor)
{
	Quaternion Ret(0.0f, 0.0f, 0.0f, 0.0f);
	VECTOR_STORE(4, &Ret, DirectX::XMQuaternionSlerp(VECTOR_LOAD(4, &Q0), VECTOR_LOAD(4, &Q1), Factor));
	return Ret;
}

inline Quaternion SlerpV(const Quaternion& Q0, const Quaternion& Q1, const Vector4& Factor)
{
	Quaternion Ret(0.0f, 0.0f, 0.0f, 0.0f);
	VECTOR_STORE(4, &Ret, DirectX::XMQuaternionSlerpV(VECTOR_LOAD(4, &Q0), VECTOR_LOAD(4, &Q1), VECTOR_LOAD(4, &Factor)));
	return Ret;
}

NAMESPACE_END(Math)
