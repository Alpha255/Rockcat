#pragma once

#include "Core/Math/Matrix.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

class Quaternion : public Float4
{
public:
	/// The DirectXMath quaternion functions use an XMVECTOR 4-vector to represent quaternions, 
	/// where the X, Y, and Z components are the vector part and the W component is the scalar part.
	Quaternion(float32_t I, float32_t J, float32_t K, float32_t S)
		: Float4(I, J, K, S)
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
		return Quaternion(x + Other.x, y + Other.y, z + Other.z, w + Other.w);
	}

	inline Quaternion& operator+=(const Quaternion& Other)
	{
		x += Other.x;
		y += Other.y;
		z += Other.z;
		w += Other.w;
		return *this;
	}

	inline Quaternion operator-(const Quaternion& Other) const
	{
		return Quaternion(x - Other.x, y - Other.y, z - Other.z, w - Other.w);
	}

	inline Quaternion& operator-=(const Quaternion& Other)
	{
		x -= Other.x;
		y -= Other.y;
		z -= Other.z;
		w -= Other.w;
		return *this;
	}

	inline Quaternion operator*(const Quaternion& Other) const
	{
		Quaternion Ret(0.0f, 0.0f, 0.0f, 0.0f);
		VECTOR_STORE(4, &Ret, DirectX::XMQuaternionMultiply(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &Other)));
		return Ret;
	}

	inline Quaternion& operator*=(const Quaternion& Other)
	{
		VECTOR_STORE(4, this, DirectX::XMQuaternionMultiply(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &Other)));
		return *this;
	}

	inline bool8_t IsNaN() const
	{
		return DirectX::XMQuaternionIsNaN(VECTOR_LOAD(4, this));
	}

	inline bool8_t IsInfinite() const
	{
		return DirectX::XMQuaternionIsInfinite(VECTOR_LOAD(4, this));
	}

	inline bool8_t IsIdentity() const
	{
		return DirectX::XMQuaternionIsIdentity(VECTOR_LOAD(4, this));
	}

	inline bool8_t operator==(const Quaternion& Other) const
	{
		return DirectX::XMQuaternionEqual(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &Other));
	}

	inline bool8_t operator!=(const Quaternion& Other) const
	{
		return DirectX::XMQuaternionNotEqual(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &Other));
	}

	inline float32_t Dot(const Quaternion& Other) const
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionDot(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &Other)));
	}

	inline float32_t Length() const
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionLength(VECTOR_LOAD(4, this)));
	}

	inline float32_t LengthSq() const
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionLengthSq(VECTOR_LOAD(4, this)));
	}

	inline float32_t ReciprocalLength() const
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionReciprocalLength(VECTOR_LOAD(4, this)));
	}

	inline static Quaternion RotationRollPitchYaw(float32_t Pitch, float32_t Yaw, float32_t Roll)
	{
		Quaternion Ret(0.0f, 0.0f, 0.0f, 0.0f);
		VECTOR_STORE(4, &Ret, DirectX::XMQuaternionRotationRollPitchYaw(Pitch, Yaw, Roll));
		return Ret;
	}

	inline static Quaternion RotationRollPitchYaw(const Vector3& PitchYawRoll)
	{
		Quaternion Ret(0.0f, 0.0f, 0.0f, 0.0f);
		VECTOR_STORE(4, &Ret, DirectX::XMQuaternionRotationRollPitchYaw(PitchYawRoll.x, PitchYawRoll.y, PitchYawRoll.z));
		return Ret;
	}

	inline static Quaternion RotationAxis(const Vector3& Axis, float32_t Angle)
	{
		Quaternion Ret(0.0f, 0.0f, 0.0f, 0.0f);
		VECTOR_STORE(4, &Ret, DirectX::XMQuaternionRotationAxis(VECTOR_LOAD(3, &Axis), Angle));
		return Ret;
	}

	inline static Quaternion RotationMatrix(const Matrix& Matrix)
	{
		Quaternion Ret(0.0f, 0.0f, 0.0f, 0.0f);
		VECTOR_STORE(4, &Ret, DirectX::XMQuaternionRotationMatrix(MATRIX_LOAD(&Matrix)));
		return Ret;
	}

	inline Matrix ToRotationMatrix()
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixRotationQuaternion(VECTOR_LOAD(4, this)));
		return Ret;
	}

	inline std::pair<Vector3, float32_t> ToAxisAngle()
	{
		DirectX::XMVECTOR AxisV;
		Vector3 Axis;
		float32_t Angle = 0.0f;
		DirectX::XMQuaternionToAxisAngle(&AxisV, &Angle, VECTOR_LOAD(4, this));
		VECTOR_STORE(3, &Axis, AxisV);
		return std::make_pair(Axis, Angle);
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

inline Quaternion Slerp(const Quaternion& Q0, const Quaternion& Q1, float32_t Factor)
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
NAMESPACE_END(Gear)
