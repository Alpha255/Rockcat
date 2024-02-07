#pragma once

#include "Core/Math/Vector/Vector2.h"

NAMESPACE_START(Math)

class Vector3 : public Float3
{
public:
    using Float3::Float3;

    inline Vector3()
        : Float3(0.0f, 0.0f, 0.0f)
    {
    }

    inline Vector3(float Value)
        : Float3(Value, Value, Value)
    {
    }

    inline Vector3(const Vector2& V)
        : Float3(V.x, V.y, 0.0f)
    {
    }

    inline Vector3& operator+=(const Vector3& Other)
    {
        DirectX::XMStoreFloat3(this, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(this), DirectX::XMLoadFloat3(&Other)));
        return *this;
    }
    inline Vector3& operator+=(float Value)
    {
        Vector3 V(Value);
        DirectX::XMStoreFloat3(this, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(this), DirectX::XMLoadFloat3(&V)));
        return *this;
    }
    friend inline Vector3 operator+(const Vector3& Left, const Vector3& Right)
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&Left), DirectX::XMLoadFloat3(&Right)));
        return Ret;
    }
    friend inline Vector3 operator+(const Vector3& Left, float Right)
    {
        Vector3 V(Right);
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&Left), DirectX::XMLoadFloat3(&V)));
        return Ret;
    }

    inline Vector3& operator-=(const Vector3& Other)
    {
        DirectX::XMStoreFloat3(this, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(this), DirectX::XMLoadFloat3(&Other)));
        return *this;
    }
    inline Vector3& operator-=(float Value)
    {
        Vector3 V(Value);
        DirectX::XMStoreFloat3(this, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(this), DirectX::XMLoadFloat3(&V)));
        return *this;
    }
    friend inline Vector3 operator-(const Vector3& Left, const Vector3& Right)
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&Left), DirectX::XMLoadFloat3(&Right)));
        return Ret;
    }
    inline friend Vector3 operator-(const Vector3& Left, float Right)
    {
        Vector3 V(Right);
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&Left), DirectX::XMLoadFloat3(&V)));
        return Ret;
    }

    inline Vector3& operator*=(const float Value)
    {
        DirectX::XMStoreFloat3(this, DirectX::XMVectorScale(DirectX::XMLoadFloat3(this), Value));
        return *this;
    }
    inline Vector3& operator*=(const Vector3& Other)
    {
        DirectX::XMStoreFloat3(this, DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(this), DirectX::XMLoadFloat3(&Other)));
        return *this;
    }
    inline friend Vector3 operator*(const Vector3& Left, const float Right)
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&Left), Right));
        return Ret;
    }
    inline friend Vector3 operator*(const Vector3& Left, const Vector3& Right)
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(&Left), DirectX::XMLoadFloat3(&Right)));
        return Ret;
    }

    inline Vector3& operator/=(const float Value)
    {
        DirectX::XMStoreFloat3(this, DirectX::XMVectorScale(DirectX::XMLoadFloat3(this), 1.0f / Value));
        return *this;
    }
    inline Vector3& operator/=(const Vector3& Other)
    {
        DirectX::XMStoreFloat3(this, DirectX::XMVectorDivide(DirectX::XMLoadFloat3(this), DirectX::XMLoadFloat3(&Other)));
        return *this;
    }
    inline friend Vector3 operator/(const Vector3& Left, const float Right)
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&Left), 1.0f / Right));
        return Ret;
    }
    inline friend Vector3 operator/(const Vector3& Left, const Vector3& Right)
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorDivide(DirectX::XMLoadFloat3(&Left), DirectX::XMLoadFloat3(&Right)));
        return Ret;
    }

    float Dot(const Vector3& Other) const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(this), DirectX::XMLoadFloat3(&Other)));
    }
    friend float Dot(const Vector3& Left, const Vector3& Right)
    {
        return DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&Left), DirectX::XMLoadFloat3(&Right)));
    }

    Vector3 Cross(const Vector3& Other) const
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(this), DirectX::XMLoadFloat3(&Other)));
        return Ret;
    }
    friend Vector3 Cross(const Vector3& Left, const Vector3& Right)
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&Left), DirectX::XMLoadFloat3(&Right)));
    }

    void Normalize()
    {
        DirectX::XMStoreFloat3(this, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(this)));
    }

    void NormalizeEst()
    {
        DirectX::XMStoreFloat3(this, DirectX::XMVector3NormalizeEst(DirectX::XMLoadFloat3(this)));
    }

    Vector3 Negate() const
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorNegate(DirectX::XMLoadFloat3(this)));
        return Ret;
    }

    float LengthSq() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMLoadFloat3(this)));
    }

    float Length() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(this)));
    }

    float LengthEst() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector3LengthEst(DirectX::XMLoadFloat3(this)));
    }
    float ReciprocalLength() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector3ReciprocalLength(DirectX::XMLoadFloat3(this)));
    }

    float ReciprocalLengthEst() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector3ReciprocalLengthEst(DirectX::XMLoadFloat3(this)));
    }

    bool IsNaN() const
    {
        return DirectX::XMVector4IsNaN(DirectX::XMLoadFloat3(this));
    }

    bool IsInfinite() const
    {
        return DirectX::XMVector4IsInfinite(DirectX::XMLoadFloat3(this));
    }

    Vector3 Round() const
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorRound(DirectX::XMLoadFloat3(this)));
        return Ret;
    }

    Vector3 Truncate() const
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorTruncate(DirectX::XMLoadFloat3(this)));
        return Ret;
    }

    Vector3 Floor() const
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorFloor(DirectX::XMLoadFloat3(this)));
        return Ret;
    }

    Vector3 Ceiling() const
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorCeiling(DirectX::XMLoadFloat3(this)));
        return Ret;
    }

    Vector3 Reciprocal() const
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorReciprocal(DirectX::XMLoadFloat3(this)));
        return Ret;
    }

    Vector3 ReciprocalEst() const
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorReciprocalEst(DirectX::XMLoadFloat3(this)));
        return Ret;
    }

    Vector3 Sqrt() const
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorSqrt(DirectX::XMLoadFloat3(this)));
        return Ret;
    }

    Vector3 SqrtEst() const
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorSqrtEst(DirectX::XMLoadFloat3(this)));
        return Ret;
    }

    Vector3 Abs() const
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVectorAbs(DirectX::XMLoadFloat3(this)));
        return Ret;
    }

    Vector3 Reflect(const Vector3& Normal) const
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVector3Reflect(DirectX::XMLoadFloat3(this), DirectX::XMLoadFloat3(&Normal)));
        return Ret;
    }

    Vector3 Refract(const Vector3& Normal, float RefractionIndex) const
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVector3Refract(DirectX::XMLoadFloat3(this), DirectX::XMLoadFloat3(&Normal), RefractionIndex));
        return Ret;
    }

    Vector3 Orthogonal() const
    {
        Vector3 Ret;
        DirectX::XMStoreFloat3(&Ret, DirectX::XMVector3Orthogonal(DirectX::XMLoadFloat3(this)));
        return Ret;
    }
};

NAMESPACE_END(Math)
