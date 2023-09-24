#pragma once

#include "Runtime/Core/Math/Vector/Vector3.h"

NAMESPACE_START(Math)

class Vector4 : public Float4
{
public:
    using Float4::Float4;

    inline Vector4()
        : Float4(0.0f, 0.0f, 0.0f, 0.0f)
    {
    }

    inline Vector4(float32_t Value)
        : Float4(Value, Value, Value, Value)
    {
    }

    inline Vector4(const Vector2& V)
        : Float4(V.x, V.y, 0.0f, 0.0f)
    {
    }

    inline Vector4(const Vector3& V)
        : Float4(V.x, V.y, V.z, 0.0f)
    {
    }

    inline Vector4& operator+=(const Vector4& Other)
    {
        DirectX::XMStoreFloat4(this, DirectX::XMVectorAdd(DirectX::XMLoadFloat4(this), DirectX::XMLoadFloat4(&Other)));
        return *this;
    }
    inline Vector4& operator+=(float32_t Value)
    {
        Vector4 V(Value);
        DirectX::XMStoreFloat4(this, DirectX::XMVectorAdd(DirectX::XMLoadFloat4(this), DirectX::XMLoadFloat4(&V)));
        return *this;
    }
    friend inline Vector4 operator+(const Vector4& Left, const Vector4& Right)
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorAdd(DirectX::XMLoadFloat4(&Left), DirectX::XMLoadFloat4(&Right)));
        return Ret;
    }
    friend inline Vector4 operator+(const Vector4& Left, float32_t Right)
    {
        Vector4 V(Right);
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorAdd(DirectX::XMLoadFloat4(&Left), DirectX::XMLoadFloat4(&V)));
        return Ret;
    }

    inline Vector4& operator-=(const Vector4& Other)
    {
        DirectX::XMStoreFloat4(this, DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(this), DirectX::XMLoadFloat4(&Other)));
        return *this;
    }
    inline Vector4& operator-=(float32_t Value)
    {
        Vector4 V(Value);
        DirectX::XMStoreFloat4(this, DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(this), DirectX::XMLoadFloat4(&V)));
        return *this;
    }
    friend inline Vector4 operator-(const Vector4& Left, const Vector4& Right)
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(&Left), DirectX::XMLoadFloat4(&Right)));
        return Ret;
    }
    inline friend Vector4 operator-(const Vector4& Left, float32_t Right)
    {
        Vector4 V(Right);
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(&Left), DirectX::XMLoadFloat4(&V)));
        return Ret;
    }

    inline Vector4& operator*=(const float32_t Value)
    {
        DirectX::XMStoreFloat4(this, DirectX::XMVectorScale(DirectX::XMLoadFloat4(this), Value));
        return *this;
    }
    inline Vector4& operator*=(const Vector4& Other)
    {
        DirectX::XMStoreFloat4(this, DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(this), DirectX::XMLoadFloat4(&Other)));
        return *this;
    }
    inline friend Vector4 operator*(const Vector4& Left, const float32_t Right)
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorScale(DirectX::XMLoadFloat4(&Left), Right));
        return Ret;
    }
    inline friend Vector4 operator*(const Vector4& Left, const Vector4& Right)
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(&Left), DirectX::XMLoadFloat4(&Right)));
        return Ret;
    }

    inline Vector4& operator/=(const float32_t Value)
    {
        DirectX::XMStoreFloat4(this, DirectX::XMVectorScale(DirectX::XMLoadFloat4(this), 1.0f / Value));
        return *this;
    }
    inline Vector4& operator/=(const Vector4& Other)
    {
        DirectX::XMStoreFloat4(this, DirectX::XMVectorDivide(DirectX::XMLoadFloat4(this), DirectX::XMLoadFloat4(&Other)));
        return *this;
    }
    inline friend Vector4 operator/(const Vector4& Left, const float32_t Right)
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorScale(DirectX::XMLoadFloat4(&Left), 1.0f / Right));
        return Ret;
    }
    inline friend Vector4 operator/(const Vector4& Left, const Vector4& Right)
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorDivide(DirectX::XMLoadFloat4(&Left), DirectX::XMLoadFloat4(&Right)));
        return Ret;
    }

    float32_t Dot(const Vector4& Other) const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector4Dot(DirectX::XMLoadFloat4(this), DirectX::XMLoadFloat4(&Other)));
    }
    friend float32_t Dot(const Vector4& Left, const Vector4& Right)
    {
        return DirectX::XMVectorGetX(DirectX::XMVector4Dot(DirectX::XMLoadFloat4(&Left), DirectX::XMLoadFloat4(&Right)));
    }

    Vector4 Cross(const Vector4& V1, const Vector4& V2) const
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVector4Cross(DirectX::XMLoadFloat4(this), DirectX::XMLoadFloat4(&V1), DirectX::XMLoadFloat4(&V2)));
        return Ret;
    }
    friend Vector4 Cross(const Vector4& V0, const Vector4& V1, const Vector4& V2)
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVector4Cross(DirectX::XMLoadFloat4(&V0), DirectX::XMLoadFloat4(&V1), DirectX::XMLoadFloat4(&V2)));
    }

    void Normalize()
    {
        DirectX::XMStoreFloat4(this, DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(this)));
    }

    void NormalizeEst()
    {
        DirectX::XMStoreFloat4(this, DirectX::XMVector4NormalizeEst(DirectX::XMLoadFloat4(this)));
    }

    Vector4 Negate() const
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorNegate(DirectX::XMLoadFloat4(this)));
        return Ret;
    }

    float32_t LengthSq() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(DirectX::XMLoadFloat4(this)));
    }

    float32_t Length() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector4Length(DirectX::XMLoadFloat4(this)));
    }

    float32_t LengthEst() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector4LengthEst(DirectX::XMLoadFloat4(this)));
    }
    float32_t ReciprocalLength() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector4ReciprocalLength(DirectX::XMLoadFloat4(this)));
    }

    float32_t ReciprocalLengthEst() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector4ReciprocalLengthEst(DirectX::XMLoadFloat4(this)));
    }

    bool8_t IsNaN() const
    {
        return DirectX::XMVector4IsNaN(DirectX::XMLoadFloat4(this));
    }

    bool8_t IsInfinite() const
    {
        return DirectX::XMVector4IsInfinite(DirectX::XMLoadFloat4(this));
    }

    Vector4 Round() const
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorRound(DirectX::XMLoadFloat4(this)));
        return Ret;
    }

    Vector4 Truncate() const
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorTruncate(DirectX::XMLoadFloat4(this)));
        return Ret;
    }

    Vector4 Floor() const
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorFloor(DirectX::XMLoadFloat4(this)));
        return Ret;
    }

    Vector4 Ceiling() const
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorCeiling(DirectX::XMLoadFloat4(this)));
        return Ret;
    }

    Vector4 Reciprocal() const
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorReciprocal(DirectX::XMLoadFloat4(this)));
        return Ret;
    }

    Vector4 ReciprocalEst() const
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorReciprocalEst(DirectX::XMLoadFloat4(this)));
        return Ret;
    }

    Vector4 Sqrt() const
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorSqrt(DirectX::XMLoadFloat4(this)));
        return Ret;
    }

    Vector4 SqrtEst() const
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorSqrtEst(DirectX::XMLoadFloat4(this)));
        return Ret;
    }

    Vector4 Abs() const
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVectorAbs(DirectX::XMLoadFloat4(this)));
        return Ret;
    }

    Vector4 Reflect(const Vector4& Normal) const
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVector4Reflect(DirectX::XMLoadFloat4(this), DirectX::XMLoadFloat4(&Normal)));
        return Ret;
    }

    Vector4 Refract(const Vector4& Normal, float32_t RefractionIndex) const
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVector4Refract(DirectX::XMLoadFloat4(this), DirectX::XMLoadFloat4(&Normal), RefractionIndex));
        return Ret;
    }

    Vector4 Orthogonal() const
    {
        Vector4 Ret;
        DirectX::XMStoreFloat4(&Ret, DirectX::XMVector4Orthogonal(DirectX::XMLoadFloat4(this)));
        return Ret;
    }
};

NAMESPACE_END(Math)
