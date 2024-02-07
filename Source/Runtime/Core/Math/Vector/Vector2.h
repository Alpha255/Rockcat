#pragma once

#include "Core/Math/Vector.h"

NAMESPACE_START(Math)

class Vector2 : public Float2
{
public:
    using Float2::Float2;

    inline Vector2()
        : Float2(0.0f, 0.0f)
    {
    }

    inline Vector2(float Value)
        : Float2(Value, Value)
    {
    }

    inline Vector2& operator+=(const Vector2& Other) 
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorAdd(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&Other)));
        return *this;
    }
    inline Vector2& operator+=(float Value)
    {
        Vector2 V(Value);
        DirectX::XMStoreFloat2(this, DirectX::XMVectorAdd(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&V)));
        return *this;
    }
    friend inline Vector2 operator+(const Vector2& Left, const Vector2& Right)
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorAdd(DirectX::XMLoadFloat2(&Left), DirectX::XMLoadFloat2(&Right)));
        return Ret;
    }
    friend inline Vector2 operator+(const Vector2& Left, float Right)
    {
        Vector2 V(Right);
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorAdd(DirectX::XMLoadFloat2(&Left), DirectX::XMLoadFloat2(&V)));
        return Ret;
    }

    inline Vector2& operator-=(const Vector2& Other)
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&Other)));
        return *this;
    }
    inline Vector2& operator-=(float Value)
    {
        Vector2 V(Value);
        DirectX::XMStoreFloat2(this, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&V)));
        return *this;
    }
    friend inline Vector2 operator-(const Vector2& Left, const Vector2& Right)
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&Left), DirectX::XMLoadFloat2(&Right)));
        return Ret;
    }
    inline friend Vector2 operator-(const Vector2& Left, float Right)
    {
        Vector2 V(Right);
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&Left), DirectX::XMLoadFloat2(&V)));
        return Ret;
    }

    inline Vector2& operator*=(const float Value)
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorScale(DirectX::XMLoadFloat2(this), Value));
        return *this;
    }
    inline Vector2& operator*=(const Vector2& Other)
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorMultiply(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&Other)));
        return *this;
    }
    inline friend Vector2 operator*(const Vector2& Left, const float Right)
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorScale(DirectX::XMLoadFloat2(&Left), Right));
        return Ret;
    }
    inline friend Vector2 operator*(const Vector2& Left, const Vector2& Right)
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorMultiply(DirectX::XMLoadFloat2(&Left), DirectX::XMLoadFloat2(&Right)));
        return Ret;
    }

    inline Vector2& operator/=(const float Value)
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorScale(DirectX::XMLoadFloat2(this), 1.0f / Value));
        return *this;
    }
    inline Vector2& operator/=(const Vector2& Other)
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorDivide(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&Other)));
        return *this;
    }
    inline friend Vector2 operator/(const Vector2& Left, const float Right)
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorScale(DirectX::XMLoadFloat2(&Left), 1.0f / Right));
        return Ret;
    }
    inline friend Vector2 operator/(const Vector2& Left, const Vector2& Right)
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorDivide(DirectX::XMLoadFloat2(&Left), DirectX::XMLoadFloat2(&Right)));
        return Ret;
    }

    float Dot(const Vector2& Other) const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2Dot(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&Other)));
    }
    friend float Dot(const Vector2& Left, const Vector2& Right)
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2Dot(DirectX::XMLoadFloat2(&Left), DirectX::XMLoadFloat2(&Right)));
    }

    Vector2 Cross(const Vector2& Other) const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVector2Cross(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&Other)));
        return Ret;
    }
    friend Vector2 Cross(const Vector2& Left, const Vector2& Right)
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVector2Cross(DirectX::XMLoadFloat2(&Left), DirectX::XMLoadFloat2(&Right)));
    }

    void Normalize()
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(this)));
    }

    void NormalizeEst()
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVector2NormalizeEst(DirectX::XMLoadFloat2(this)));
    }

    Vector2 Negate() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorNegate(DirectX::XMLoadFloat2(this)));
        return Ret;
    }

    float LengthSq() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(DirectX::XMLoadFloat2(this)));
    }

    float Length() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMLoadFloat2(this)));
    }

    float LengthEst() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2LengthEst(DirectX::XMLoadFloat2(this)));
    }
    float ReciprocalLength() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2ReciprocalLength(DirectX::XMLoadFloat2(this)));
    }

    float ReciprocalLengthEst() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2ReciprocalLengthEst(DirectX::XMLoadFloat2(this)));
    }

    bool IsNaN() const
    {
        return DirectX::XMVector4IsNaN(DirectX::XMLoadFloat2(this));
    }

    bool IsInfinite() const
    {
        return DirectX::XMVector4IsInfinite(DirectX::XMLoadFloat2(this));
    }

    Vector2 Round() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorRound(DirectX::XMLoadFloat2(this)));
        return Ret;
    }

    Vector2 Truncate() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorTruncate(DirectX::XMLoadFloat2(this)));
        return Ret;
    }

    Vector2 Floor() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorFloor(DirectX::XMLoadFloat2(this)));
        return Ret;
    }

    Vector2 Ceiling() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorCeiling(DirectX::XMLoadFloat2(this)));
        return Ret;
    }

    Vector2 Reciprocal() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorReciprocal(DirectX::XMLoadFloat2(this)));
        return Ret;
    }

    Vector2 ReciprocalEst() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorReciprocalEst(DirectX::XMLoadFloat2(this)));
        return Ret;
    }

    Vector2 Sqrt() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorSqrt(DirectX::XMLoadFloat2(this)));
        return Ret;
    }

    Vector2 SqrtEst() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorSqrtEst(DirectX::XMLoadFloat2(this)));
        return Ret;
    }

    Vector2 Abs() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorAbs(DirectX::XMLoadFloat2(this)));
        return Ret;
    }

    Vector2 Reflect(const Vector2& Normal) const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVector2Reflect(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&Normal)));
        return Ret;
    }

    Vector2 Refract(const Vector2& Normal, float RefractionIndex) const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVector2Refract(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&Normal), RefractionIndex));
        return Ret;
    }

    Vector2 Orthogonal() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVector2Orthogonal(DirectX::XMLoadFloat2(this)));
        return Ret;
    }
};

NAMESPACE_END(Math)
