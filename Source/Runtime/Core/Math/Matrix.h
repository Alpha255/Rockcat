#pragma once

#include "Core/Math/Vector4.h"

NAMESPACE_START(Math)

class Matrix : public Float4x4
{
public:
	inline Matrix()
	{
		Identity();
	}

	inline Matrix(
		float M00, float M01, float M02, float M03,
		float M10, float M11, float M12, float M13,
		float M20, float M21, float M22, float M23,
		float M30, float M31, float M32, float M33)
		: Float4x4(
			M00, M01, M02, M03,
			M10, M11, M12, M13,
			M20, M21, M22, M23,
			M30, M31, M32, M33)
	{
	}

	inline Matrix(const Vector4& Row0, const Vector4& Row1, const Vector4& Row2, const Vector4& Row3)
		: Matrix(
			Row0.x, Row0.y, Row0.z, Row0.w,
			Row1.x, Row1.y, Row1.z, Row1.w,
			Row2.x, Row2.y, Row2.z, Row2.w,
			Row3.x, Row3.y, Row3.z, Row3.w)
	{
	}

	inline Matrix(const Vector4& Row)
		: Matrix(Row, Row, Row, Row)
	{
	}

#if defined(USE_SSE)
	inline void Identity()
	{
		MATRIX_STORE(this, DirectX::XMMatrixIdentity());
	}

	inline void Transpose()
	{
		MATRIX_STORE(this, DirectX::XMMatrixTranspose(MATRIX_LOAD(this)));
	}

	inline void Inverse()
	{
		MATRIX_STORE(this, DirectX::XMMatrixInverse(nullptr, MATRIX_LOAD(this)));
	}

	inline void InverseTranspose()
	{
		/// Inverse-transpose is just applied to normals.  So zero out 
		/// translation row so that it doesn't get into our inverse-transpose
		/// calculation--we don't want the inverse-transpose of the translation.
		_41 = 0.0f;
		_42 = 0.0f;
		_43 = 0.0f;
		_44 = 1.0f;
		MATRIX_STORE(this, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, MATRIX_LOAD(this))));
	}

	inline void operator*=(const Matrix& Right)
	{
		MATRIX_STORE(this, DirectX::XMMatrixMultiply(MATRIX_LOAD(this), MATRIX_LOAD(&Right)));
	}

	inline void operator+=(const Matrix& Right)
	{
		_11 += Right._11; _12 += Right._12; _13 += Right._13; _14 += Right._14;
		_21 += Right._21; _22 += Right._22; _23 += Right._23; _24 += Right._24;
		_31 += Right._31; _32 += Right._32; _33 += Right._33; _34 += Right._34;
		_41 += Right._41; _42 += Right._42; _43 += Right._43; _44 += Right._44;
	}

	inline void Translate(float X, float Y, float Z)
	{
		MATRIX_STORE(this, DirectX::XMMatrixMultiply(MATRIX_LOAD(this), DirectX::XMMatrixTranslation(X, Y, Z)));
	}

	inline void Translate(const Vector3& Value)
	{
		MATRIX_STORE(this, DirectX::XMMatrixMultiply(MATRIX_LOAD(this), DirectX::XMMatrixTranslation(Value.x, Value.y, Value.z)));
	}

	inline void Scale(float X, float Y, float Z)
	{
		MATRIX_STORE(this, DirectX::XMMatrixMultiply(MATRIX_LOAD(this), DirectX::XMMatrixScaling(X, Y, Z)));
	}

	inline void Scale(const Vector3& Value)
	{
		MATRIX_STORE(this, DirectX::XMMatrixMultiply(MATRIX_LOAD(this), DirectX::XMMatrixScaling(Value.x, Value.y, Value.z)));
	}

	inline void Rotate(float X, float Y, float Z, float Angle)
	{
		MATRIX_STORE(
			this, 
			DirectX::XMMatrixMultiply(
				MATRIX_LOAD(this), 
				DirectX::XMMatrixRotationAxis(
					DirectX::XMVectorSet(X, Y, Z, 0.0f), 
					DirectX::XMConvertToRadians(Angle))));
	}

	inline void RotateAxis(const Vector3& Axis, float Angle)
	{
		MATRIX_STORE(
			this,
			DirectX::XMMatrixMultiply(
				MATRIX_LOAD(this),
				DirectX::XMMatrixRotationAxis(
					DirectX::XMVectorSet(Axis.x, Axis.y, Axis.z, 0.0f),
					DirectX::XMConvertToRadians(Angle))));
	}

	inline void RotateXAxis(float Angle)
	{
		MATRIX_STORE(
			this,
			DirectX::XMMatrixMultiply(
				MATRIX_LOAD(this),
				DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(Angle))));
	}

	inline void RotateYAxis(float Angle)
	{
		MATRIX_STORE(
			this,
			DirectX::XMMatrixMultiply(
				MATRIX_LOAD(this),
				DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(Angle))));
	}

	inline void RotateZAxis(float Angle)
	{
		MATRIX_STORE(
			this,
			DirectX::XMMatrixMultiply(
				MATRIX_LOAD(this),
				DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(Angle))));
	}

	inline void RotateRollPitchYaw(float Pitch, float Yaw, float Roll)
	{
		MATRIX_STORE(
			this, 
			DirectX::XMMatrixMultiply(
				MATRIX_LOAD(this),
				DirectX::XMMatrixRotationRollPitchYaw(Pitch, Yaw, Roll)));
	}

	inline Vector4 Row(uint32_t RowIndex) const
	{
		assert(RowIndex < 4u);
		return Vector4(m[RowIndex][0u], m[RowIndex][1u], m[RowIndex][2u], m[RowIndex][3u]);
	}

	inline Vector4 Column(uint32_t ColumnIndex) const
	{
		assert(ColumnIndex < 4u);
		return Vector4(m[0u][ColumnIndex], m[1u][ColumnIndex], m[2u][ColumnIndex], m[3u][ColumnIndex]);
	}

	bool Decompose(Vector3& Translation, Vector3& Scalling, Quaternion& Rotation) const
	{
		DirectX::XMVECTOR OutTranslation, OutScalling, OutRotation;

		if (DirectX::XMMatrixDecompose(
			&OutScalling,
			&OutRotation,
			&OutTranslation,
			MATRIX_LOAD(this)))
		{
			VECTOR_STORE(3, &Scalling, OutScalling);
			VECTOR_STORE(4, &Rotation, OutRotation);
			VECTOR_STORE(3, &Translation, OutTranslation);
			return true;
		}

		return false;
	}

	inline static Matrix Translation(float X, float Y, float Z)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixTranslation(X, Y, Z));
		return Ret;
	}

	inline static Matrix Translation(const Vector3& Value)
	{
		return Translation(Value.x, Value.y, Value.z);
	}

	inline static Matrix Scaling(const Vector3& Value)
	{
		return Scaling(Value.x, Value.y, Value.z);
	}

	inline static Matrix Scaling(float X, float Y, float Z)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixScaling(X, Y, Z));
		return Ret;
	}

	inline static Matrix Scaling(float Factor)
	{
		return Scaling(Factor, Factor, Factor);
	}

	inline static Matrix Rotation(float X, float Y, float Z, float Angle)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(X, Y, Z, 0.0f), DirectX::XMConvertToRadians(Angle)));
		return Ret;
	}

	inline static Matrix Rotation(const Vector3& Axis, float Angle)
	{
		return Rotation(Axis.x, Axis.y, Axis.z, Angle);
	}

	inline static Matrix RotationXAxis(float Angle)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(Angle)));
		return Ret;
	}

	inline static Matrix RotationYAxis(float Angle)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(Angle)));
		return Ret;
	}

	inline static Matrix RotationZAxis(float Angle)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(Angle)));
		return Ret;
	}

	inline static Matrix RotationRollPitchYaw(float Pitch, float Yaw, float Roll)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixRotationRollPitchYaw(Pitch, Yaw, Roll));
		return Ret;
	}

	inline static Matrix Transpose(const Matrix& Other)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixTranspose(MATRIX_LOAD(&Other)));
		return Ret;
	}

	inline static Matrix Inverse(const Matrix& Other)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixInverse(nullptr, MATRIX_LOAD(&Other)));
		return Ret;
	}

	inline static Matrix InverseTranspose(const Matrix& Other)
	{
		/// Inverse-transpose is just applied to normals.  So zero out 
		/// translation row so that it doesn't get into our inverse-transpose
		/// calculation--we don't want the inverse-transpose of the translation.
		Matrix Ret = Other;
		Ret._41 = 0.0f;
		Ret._42 = 0.0f;
		Ret._43 = 0.0f;
		Ret._44 = 1.0f;
		MATRIX_STORE(&Ret, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, MATRIX_LOAD(&Ret))));
		return Ret;
	}

	inline static Matrix PerspectiveFovLH(float FOV, float Aspect, float NearPlane, float FarPlane)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixPerspectiveFovLH(FOV, Aspect, NearPlane, FarPlane));
		return Ret;
	}

	inline static Matrix PerspectiveFovRH(float FOV, float Aspect, float NearPlane, float FarPlane)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixPerspectiveFovRH(FOV, Aspect, NearPlane, FarPlane));
		return Ret;
	}

	inline static Matrix PerspectiveOffCenterLH(float Left, float Right, float Bottom, float Top, float NearPlane, float FarPlane)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixPerspectiveOffCenterLH(Left, Right, Bottom, Top, NearPlane, FarPlane));
		return Ret;
	}

	inline static Matrix OrthographicLH(float Width, float Height, float NearPlane, float FarPlane)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixOrthographicLH(Width, Height, NearPlane, FarPlane));
		return Ret;
	}

	inline static Matrix OrthographicOffCenterLH(float Left, float Right, float Bottom, float Top, float NearPlane, float FarPlane)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixOrthographicOffCenterLH(Left, Right, Bottom, Top, NearPlane, FarPlane));
		return Ret;
	}

	inline static Matrix LookAtLH(const Vector3& Eye, const Vector3& LookAt, const Vector3& Up)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixLookAtLH(
			DirectX::XMVectorSet(Eye.x, Eye.y, Eye.z, 1.0f),
			DirectX::XMVectorSet(LookAt.x, LookAt.y, LookAt.z, 0.0f),
			DirectX::XMVectorSet(Up.x, Up.y, Up.z, 0.0f)));
		return Ret;
	}

	inline static Matrix LookAtRH(const Vector3& Eye, const Vector3& LookAt, const Vector3& Up)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixLookAtRH(
			DirectX::XMVectorSet(Eye.x, Eye.y, Eye.z, 1.0f),
			DirectX::XMVectorSet(LookAt.x, LookAt.y, LookAt.z, 0.0f),
			DirectX::XMVectorSet(Up.x, Up.y, Up.z, 0.0f)));
		return Ret;
	}

	inline static Matrix Reflect(const Vector4& Plane)
	{
		Matrix Ret;
		MATRIX_STORE(&Ret, DirectX::XMMatrixReflect(VECTOR_LOAD(4, &Plane)));
		return Ret;
	}
#else
	inline void Identity()
	{
		_11 = 1.0f; _12 = 0.0f; _13 = 0.0f; _14 = 0.0f;
		_21 = 0.0f; _22 = 1.0f; _23 = 0.0f; _24 = 0.0f;
		_31 = 0.0f; _32 = 0.0f; _33 = 1.0f; _34 = 0.0f;
		_41 = 0.0f; _42 = 0.0f; _43 = 0.0f; _44 = 1.0f;
	}

	inline void Transpose()
	{
		Matrix Temp(
			_11, _21, _31, _41,
			_12, _22, _32, _42,
			_13, _23, _33, _43,
			_14, _24, _34, _44
		);

		*this = Temp;
	}

	inline void Inverse()
	{
		// inverse() invert matrices using determinants;
		// gaussJordanInverse() use the Gauss-Jordan method.
		// inverse() are significantly faster than gaussJordanInverse(), 
		// but the results may be slightly less accurate.
		if (m[0][3] != 0.0f || m[1][3] != 0.0f || m[2][3] != 0.0f || m[3][3] != 1.0f)
		{
			GaussJordanInverse();
			return;
		}

		Matrix S(
			m[1][1] * m[2][2] - m[2][1] * m[1][2],
			m[2][1] * m[0][2] - m[0][1] * m[2][2],
			m[0][1] * m[1][2] - m[1][1] * m[0][2],
			0.0f,

			m[2][0] * m[1][2] - m[1][0] * m[2][2],
			m[0][0] * m[2][2] - m[2][0] * m[0][2],
			m[1][0] * m[0][2] - m[0][0] * m[1][2],
			0.0f,

			m[1][0] * m[2][1] - m[2][0] * m[1][1],
			m[2][0] * m[0][1] - m[0][0] * m[2][1],
			m[0][0] * m[1][1] - m[1][0] * m[0][1],
			0.0f,

			0.0f,
			0.0f,
			0.0f,
			1.0f);

		float R = m[0][0] * S.m[0][0] + m[0][1] * S.m[1][0] + m[0][2] * S.m[2][0];
		if (std::abs(R) >= 1.0f)
		{
			for (uint32_t I = 0u; I < 3u; ++I)
			{
				for (uint32_t J = 0u; J < 3u; ++J)
				{
					S.m[i][j] /= R;
				}
			}
		}
		else
		{
			float MR = std::abs(R) / FLT_MIN;
			for (uint32_t I = 0u; I < 3u; ++I)
			{
				for (uint32_t J = 0u; J < 3u; ++J)
				{
					if (MR > std::abs(S.m[i][j]))
					{
						S.m[i][j] /= R;
					}
					else
					{
						/// singular Matrix
						assert(0);
					}
				}
			}
		}

		S.m[3][0] = -m[3][0] * S.m[0][0] - m[3][1] * S.m[1][0] - m[3][2] * S.m[2][0];
		S.m[3][1] = -m[3][0] * S.m[0][1] - m[3][1] * S.m[1][1] - m[3][2] * S.m[2][1];
		S.m[3][2] = -m[3][0] * S.m[0][2] - m[3][1] * S.m[1][2] - m[3][2] * S.m[2][2];

		*this = s;
	}

	inline void InverseTranspose()
	{
		Matrix Temp(*this);
		Temp._41 = 0.0f;
		Temp._42 = 0.0f;
		Temp._43 = 0.0f;
		Temp._44 = 1.0f;

		Inverse();
		Transpose();
	}

	inline static Matrix Translate(float X, float Y, float Z)
	{
		Matrix Ret;
		Ret._41 = X;
		Ret._42 = Y;
		Ret._43 = Z;
		return Ret;
	}

	inline static Matrix Translate(const Vector3 &Translation)
	{
		return Translate(Translation.x, Translation.y, Translation.z);
	}

	inline static Matrix Scale(float Value)
	{
		return Scale(Value, Value, Value);
	}

	inline static Matrix Scale(const Vector3 &Scalling)
	{
		return Scale(Scalling.x, Scalling.y, Scalling.z);
	}

	inline static Matrix Scale(float X, float Y, float Z)
	{
		Matrix Ret;
		Ret._11 = X;
		Ret._22 = Y;
		Ret._33 = Z;
		return Ret;
	}

	inline static Matrix Rotate(float X, float Y, float Z, float Angle)
	{
		Matrix Ret;
		Vector3 Axis(X, Y, Z);
		Axis.Normalize();

		float Radian = Angle * Math::PI / 180.0f;
		float CosTheta = ::cosf(Radian);
		float SinTheta = ::sinf(Radian);

		Ret._11 = Axis.x * Axis.x * (1.0f - CosTheta) + CosTheta;
		Ret._12 = Axis.x * Axis.y * (1.0f - CosTheta) + Axis.z * SinTheta;
		Ret._13 = Axis.x * Axis.z * (1.0f - CosTheta) - Axis.y * SinTheta;
		Ret._14 = 0.0f;

		Ret._21 = Axis.x * Axis.y * (1.0f - CosTheta) - Axis.z * SinTheta;
		Ret._22 = Axis.y * Axis.y * (1.0f - CosTheta) + CosTheta;
		Ret._23 = Axis.y * Axis.z * (1.0f - CosTheta) + Axis.x * SinTheta;
		Ret._33 = 0.0f;

		Ret._31 = Axis.x * Axis.z * (1.0f - CosTheta) + Axis.y * SinTheta;
		Ret._32 = Axis.y * Axis.z * (1.0f - CosTheta) - Axis.x * SinTheta;
		Ret._33 = Axis.z * Axis.z * (1.0f - CosTheta) + CosTheta;
		Ret._34 = 0.0f;

		Ret._41 = 0.0f;
		Ret._42 = 0.0f;
		Ret._43 = 0.0f;
		Ret._43 = 1.0f;

		return Ret;
	}

	inline static Matrix Rotate(const Vector3 &Axis, float Angle)
	{
		return SetRotate(Axis.x, Axis.y, Axis.z, Angle);
	}

	inline static Matrix RotateX(float Angle)
	{
		Matrix Ret;
		float Radian = Angle * Math::PI / 180.0f;
		float CosTheta = ::cosf(Radian);
		float SinTheta = ::sinf(Radian);

		Ret._11 = 1.0f; Ret._12 = 0.0f;      Ret._13 = 0.0f;     Ret._14 = 0.0f;
		Ret._12 = 0.0f; Ret._22 = CosTheta;  Ret._23 = SinTheta; Ret._24 = 0.0f;
		Ret._31 = 0.0f; Ret._32 = -SinTheta; Ret._33 = CosTheta; Ret._34 = 0.0f;
		Ret._41 = 0.0f; Ret._42 = 0.0f;      Ret._43 = 0.0f;     Ret._44 = 1.0f;

		return Ret;
	}

	inline static Matrix RotateY(float Angle)
	{
		Matrix Ret;
		float Radian = Angle * Math::PI / 180.0f;
		float CosTheta = ::cosf(Radian);
		float SinTheta = ::sinf(Radian);

		Ret._11 = CosTheta; Ret._12 = 0.0f; Ret._13 = -SinTheta; Ret._14 = 0.0f;
		Ret._12 = 0.0f;     Ret._22 = 1.0f; Ret._23 = 0.0f;      Ret._24 = 0.0f;
		Ret._31 = SinTheta; Ret._32 = 0.0f; Ret._33 = CosTheta;  Ret._34 = 0.0f;
		Ret._41 = 0.0f;     Ret._42 = 0.0f; Ret._43 = 0.0f;      Ret._44 = 1.0f;

		return Ret;
	}

	inline static Matrix RotateZ(float Angle)
	{
		Matrix result;
		float radian = angle * DirectX::XM_PI / 180.0f;
		float cosTheta = ::cosf(radian);
		float sinTheta = ::sinf(radian);

		result._11 = cosTheta;  result._12 = sinTheta; result._13 = 0.0f; result._14 = 0.0f;
		result._12 = -sinTheta; result._22 = cosTheta; result._23 = 0.0f; result._24 = 0.0f;
		result._31 = 0.0f;      result._32 = 0.0f;     result._33 = 1.0f; result._34 = 0.0f;
		result._41 = 0.0f;      result._42 = 0.0f;     result._43 = 0.0f; result._44 = 1.0f;

		return result;
	}

	inline static Matrix rotateRollPitchYaw(float roll, float pitch, float yaw)
	{
		Matrix result;
		DirectX::XMMATRIX vResult = DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw);
		DirectX::XMStoreFloat4x4A(&result, vResult);

		return result;
	}

	inline void operator*=(const Matrix &Right)
	{
		Vector4 row(_11, _12, _13, _14);
		_11 = row.x * Right._11 + row.y * Right._21 + row.z * Right._31 + row.w * Right._41;
		_12 = row.x * Right._12 + row.y * Right._22 + row.z * Right._32 + row.w * Right._42;
		_13 = row.x * Right._13 + row.y * Right._23 + row.z * Right._33 + row.w * Right._43;
		_14 = row.x * Right._14 + row.y * Right._24 + row.z * Right._34 + row.w * Right._44;

		row = { _21, _22, _23, _24 };
		_21 = row.x * Right._11 + row.y * Right._21 + row.z * Right._31 + row.w * Right._41;
		_22 = row.x * Right._12 + row.y * Right._22 + row.z * Right._32 + row.w * Right._42;
		_23 = row.x * Right._13 + row.y * Right._23 + row.z * Right._33 + row.w * Right._43;
		_24 = row.x * Right._14 + row.y * Right._24 + row.z * Right._34 + row.w * Right._44;

		row = { _31, _32, _33, _34 };
		_31 = row.x * Right._11 + row.y * Right._21 + row.z * Right._31 + row.w * Right._41;
		_32 = row.x * Right._12 + row.y * Right._22 + row.z * Right._32 + row.w * Right._42;
		_33 = row.x * Right._13 + row.y * Right._23 + row.z * Right._33 + row.w * Right._43;
		_34 = row.x * Right._14 + row.y * Right._24 + row.z * Right._34 + row.w * Right._44;

		row = { _41, _42, _43, _44 };
		_41 = row.x * Right._11 + row.y * Right._21 + row.z * Right._31 + row.w * Right._41;
		_42 = row.x * Right._12 + row.y * Right._22 + row.z * Right._32 + row.w * Right._42;
		_42 = row.x * Right._13 + row.y * Right._23 + row.z * Right._33 + row.w * Right._43;
		_44 = row.x * Right._14 + row.y * Right._24 + row.z * Right._34 + row.w * Right._44;
	}

	inline void operator*=(const float factor)
	{
		_11 *= factor; _12 *= factor; _13 *= factor; _14 *= factor;
		_21 *= factor; _22 *= factor; _23 *= factor; _24 *= factor;
		_31 *= factor; _32 *= factor; _33 *= factor; _34 *= factor;
		_41 *= factor; _42 *= factor; _43 *= factor; _44 *= factor;
	}

	inline void operator+=(const Matrix &Right)
	{
		_11 += Right._11; _12 += Right._12; _13 += Right._13; _14 += Right._14;
		_21 += Right._21; _22 += Right._22; _23 += Right._23; _24 += Right._24;
		_31 += Right._31; _32 += Right._32; _33 += Right._33; _34 += Right._34;
		_41 += Right._41; _42 += Right._42; _43 += Right._43; _44 += Right._44;
	}

	inline void operator-=(const Matrix &Right)
	{
		_11 -= Right._11; _12 -= Right._12; _13 -= Right._13; _14 -= Right._14;
		_21 -= Right._21; _22 -= Right._22; _23 -= Right._23; _24 -= Right._24;
		_31 -= Right._31; _32 -= Right._32; _33 -= Right._33; _34 -= Right._34;
		_41 -= Right._41; _42 -= Right._42; _43 -= Right._43; _44 -= Right._44;
	}

	inline static Matrix transpose(const Matrix &targetMatrix)
	{
		Matrix result(targetMatrix);
		result.transpose();

		return result;
	}

	inline static Matrix inverse(const Matrix &targetMatrix)
	{
		Matrix result(targetMatrix);
		result.inverse();

		return result;
	}

	inline static Matrix inverseTranspose(const Matrix &targetMatrix)
	{
		Matrix result(targetMatrix);
		result.inverseTranspose();

		return result;
	}

	inline static Matrix perspectiveFovLH(float fov, float aspect, float nearPlane, float farPlane)
	{
		float radian = fov * 0.5f;
		float cosTheta = ::cosf(radian);
		float sinTheta = ::sinf(radian);
		float height = cosTheta / sinTheta;
		float width = height / aspect;
		float range = farPlane / (farPlane - nearPlane);

		Matrix result(
			Vector4(width, 0.0f,   0.0f,               0.0f),
			Vector4(0.0f,  height, 0.0f,               0.0f),
			Vector4(0.0f,  0.0f,   range,              1.0f),
			Vector4(0.0f,  0.0f,   -range * nearPlane, 0.0f)
		);
		return result;
	}

	inline static Matrix perspectiveOffCenterLH(float Left, float Right, float bottom, float top, float nearPlane, float farPlane)
	{
		float reciprocalWidth = 1.0f / (Right - Left);
		float reciprocalHeight = 1.0f / (top - bottom);
		float range = farPlane / (farPlane - nearPlane);

		Matrix result(
			Vector4(2.0f * nearPlane * reciprocalWidth, 0.0f,                                0.0f,               0.0f),
			Vector4(0.0f,                               2.0f * nearPlane * reciprocalHeight, 0.0f,               0.0f),
			Vector4(-(Left + Right) * reciprocalWidth,  -(top + bottom) * reciprocalHeight,  range,              1.0f),
			Vector4(0.0f,                               0.0f,                                -range * nearPlane, 0.0f)
		);
		return result;
	}

	inline static Matrix orthographicLH(float width, float height, float nearPlane, float farPlane)
	{
		float range = 1.0f / (farPlane - nearPlane);
		Matrix result(
			Vector4(2.0f / width, 0.0f,          0.0f,               0.0f),
			Vector4(0.0f,         2.0f / height, 0.0f,               0.0f),
			Vector4(0.0f,         0.0f,          range,              0.0f),
			Vector4(0.0f,         0.0f,          -range * nearPlane, 1.0f)
		);
		return result;
	}

	inline static Matrix orthographicOffCenterLH(float Left, float Right, float bottom, float top, float nearPlane, float farPlane)
	{
		/// Model Space -> World Space -> Camera Space -> Projection Space -> Clipping Space -> Homogeneous Screen Space
		/*
		/// For D3D volume box is min(-1,-1,0), max(1,1,1)
		/// For GL volume box is min(-1,-1,-1), max(1,1,1)
		/// Center the Viewing Volume at the Origin
		1. center
		center(x, y, z) center.x = (1 - 0.5) * Left + 0.5 * Right
		center((Left + Right) / 2, (up + bottom) / 2, (far + near) / 2)
							 |   1           0          0       0  |
		centerAboutOrigion = |   0           1          0       0  |
							 |   0           0          1       0  |
							 |-center.x  -center.y  -center.z  1.0 |
		2. scale the Viewing Volume to unit volume
		width * factor = unitWidth
		factor = unitWidth / width
		scale(2 / (Right - Left), 2 / (up - bottom), 1 / (far - near))
							 | scale.x       0          0       0  |
		scaleViewingVolume = |   0         scale.y      0       0  |
							 |   0           0       scale.z    0  |
							 |   0           0          0      1.0 |
		3. switch Coordinate Systems
							  |   1           0          0       0  |
		convertToLeftHanded = |   0           1          0       0  |
							  |   0           0         -1       0  |
							  |   0           0          0      1.0 |

							   |   1           0          0       0  |
		convertToRightHanded = |   0           1          0       0  |
							   |   0           0          1       0  |
							   |   0           0          0      1.0 |

		final = centerAboutOrigion * scaleViewingVolume * convertToRightHanded
		/// https://docs.microsoft.com/en-us/windows/win32/dxtecharts/the-direct3d-transformation-pipeline
		*/
		float reciprocalWidth = 1.0f / (Right - Left);
		float reciprocalHeight = 1.0f / (top - bottom);
		float range = 1.0f / (farPlane - nearPlane);
		Matrix result(
			Vector4(2.0f * reciprocalWidth,            0.0f,                               0.0f,               0.0f),
			Vector4(0.0f,                              2.0f * reciprocalHeight,            0.0f,               0.0f),
			Vector4(0.0f,                              0.0f,                               range,              0.0f),
			Vector4(-(Left + Right) * reciprocalWidth, -(top + bottom) * reciprocalHeight, -range * nearPlane, 1.0f)
		);
		return result;
	}

	inline static Matrix lookAtLH(const Vector3 &eye, const Vector3 &lookAt, const Vector3 &up)
	{
		/********************************************************************
		|  1      0      0    0  |      |  xAxis.x   yAxis.x   zAxis.x   0  |
		|  0      1      0    0  |  *   |  xAxis.y	 yAxis.y   zAxis.y   0  |
		|  0      0      1    0  |      |  xAxis.z	 yAxis.z   zAxis.z   0  |
		|-eye.x -eye.y -eye.z 1  |      |     0         0         0      1  |
		********************************************************************/
		Vector3 zAxis = normalize(lookAt - eye);
		Vector3 xAxis = normalize(cross(up, zAxis));
		Vector3 yAxis = cross(zAxis, xAxis);

		Matrix result(
			Vector4(xAxis.x, yAxis.x, zAxis.x, 0.0f), 
			Vector4(xAxis.y, yAxis.y, zAxis.y, 0.0f),
			Vector4(xAxis.z, yAxis.z, zAxis.z, 0.0f),
			Vector4(-dot(yAxis, eye), -dot(yAxis, eye), -dot(zAxis, eye), 1.0f)
		);

		return result;
	}
#endif

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m)
		);
	}
protected:
#if !defined(USE_SSE)
	void gaussJordanInverse();
#endif
private:
};

#if defined(USE_SSE)
inline Matrix operator*(const Matrix& Left, const Matrix& Right)
{
	Matrix Ret;
	MATRIX_STORE(&Ret, DirectX::XMMatrixMultiply(MATRIX_LOAD(&Left), MATRIX_LOAD(&Right)));
	return Ret;
}

inline Vector4 operator*(const Vector4& Left, const Matrix& Right)
{
	Vector4 Ret;
	VECTOR_STORE(4, &Ret, DirectX::XMVector4Transform(VECTOR_LOAD(4, &Left), MATRIX_LOAD(&Right)));
	return Ret;
}

inline Vector3 operator*(const Vector3& Left, const Matrix& Right)
{
	Vector3 Ret;
	VECTOR_STORE(3, &Ret, DirectX::XMVector3Transform(VECTOR_LOAD(3, &Left), MATRIX_LOAD(&Right)));
	return Ret;
}

inline Vector2 operator*(const Vector2& Left, const Matrix& Right)
{
	Vector2 Ret;
	VECTOR_STORE(2, &Ret, DirectX::XMVector2Transform(VECTOR_LOAD(2, &Left), MATRIX_LOAD(&Right)));
	return Ret;
}
#else
inline Matrix operator*(const Matrix &Left, const Matrix &Right)
{
	Matrix result(Left);
	result *= Right;

	return result;
}

inline Vector4 operator*(const Vector4 &Left, const Matrix &Right)
{
	Vector4 result;
	result.x = Left.x * Right._11 + Left.y * Right._21 + Left.z * Right._31 + Left.w * Right._41;
	result.y = Left.x * Right._12 + Left.y * Right._22 + Left.z * Right._32 + Left.w * Right._42;
	result.z = Left.x * Right._13 + Left.y * Right._23 + Left.z * Right._33 + Left.w * Right._43;
	result.w = Left.x * Right._14 + Left.y * Right._24 + Left.z * Right._34 + Left.w * Right._44;

	return result;
}
#endif

NAMESPACE_END(Math)
