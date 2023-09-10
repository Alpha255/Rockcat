#pragma once

#include "Runtime/Core/Math/Vector.h"

NAMESPACE_START(Math)

#if 0
class Vector
{
public:
    Vector()
    {
        DirectX::XMVectorReplicate(0.0f);
    }

    Vector(const Vector& Other)
        : m_Value(Other.m_Value)
    {
    }

    Vector& operator=(const Vector& Other)
    {
        m_Value = Other.m_Value;
        return *this;
    }

    explicit Vector(float32_t Value)
    {
        DirectX::XMVectorReplicate(Value);
    }

    explicit Vector(const float32_t* Values)
    {
        DirectX::XMVectorReplicatePtr(Values);
    }

    explicit Vector(uint32_t Value)
    {
        DirectX::XMVectorReplicateInt(Value);
    }

    explicit Vector(const uint32_t* Values)
    {
        DirectX::XMVectorReplicateIntPtr(Values);
    }

    bool8_t IsNaN() const
    {
        return DirectX::XMVector4IsNaN(m_Value);
    }

    bool8_t IsInfinite() const
    {
        return DirectX::XMVector4IsInfinite(m_Value);
    }

    bool8_t operator==(const Vector& Other) const
    {
        return DirectX::XMVector4Equal(m_Value, Other.m_Value);
    }

    bool8_t operator!=(const Vector& Other) const
    {
        return DirectX::XMVector4NotEqual(m_Value, Other.m_Value);
    }

    bool8_t operator>(const Vector& Other) const
    {
        return DirectX::XMVector4Greater(m_Value, Other.m_Value);
    }

    bool8_t operator>=(const Vector& Other) const
    {
        return DirectX::XMVector4GreaterOrEqual(m_Value, Other.m_Value);
    }

    bool8_t operator<(const Vector& Other) const
    {
        return DirectX::XMVector4Less(m_Value, Other.m_Value);
    }

    bool8_t operator<=(const Vector& Other) const
    {
        return DirectX::XMVector4LessOrEqual(m_Value, Other.m_Value);
    }

    Vector Round() const
    {
        return DirectX::XMVectorRound(m_Value);
    }

    Vector Truncate() const
    {
        return DirectX::XMVectorTruncate(m_Value);
    }

    Vector Floor() const
    {
        return DirectX::XMVectorFloor(m_Value);
    }

    Vector Ceiling() const
    {
        return DirectX::XMVectorCeiling(m_Value);
    }

    Vector Saturate() const
    {
        return DirectX::XMVectorSaturate(m_Value);
    }

    Vector Negate() const
    {
        return DirectX::XMVectorNegate(m_Value);
    }

    Vector Scale(float32_t Factor) const
    {
        return DirectX::XMVectorScale(m_Value, Factor);
    }

    Vector Reciprocal() const
    {
        return DirectX::XMVectorReciprocal(m_Value);
    }

    Vector ReciprocalEst() const
    {
        return DirectX::XMVectorReciprocalEst(m_Value);
    }

    Vector Sqrt() const
    {
        return DirectX::XMVectorSqrt(m_Value);
    }

    Vector SqrtEst() const
    {
        return DirectX::XMVectorSqrtEst(m_Value);
    }

    Vector Abs() const
    {
        return DirectX::XMVectorAbs(m_Value);
    }

    Vector& operator+=(const Vector& Other)
    {
        m_Value = DirectX::XMVectorAdd(m_Value, Other.m_Value);
        return *this;
    }

    Vector& operator-=(const Vector& Other)
    {
        m_Value = DirectX::XMVectorSubtract(m_Value, Other.m_Value);
        return *this;
    }

    Vector& operator*=(const Vector& Other)
    {
        m_Value = DirectX::XMVectorMultiply(m_Value, Other.m_Value);
        return *this;
    }

    Vector& operator*=(float32_t Factor)
    {
        m_Value = DirectX::XMVectorScale(m_Value, Factor);
        return *this;
    }

    Vector& operator/=(const Vector& Other)
    {
        m_Value = DirectX::XMVectorDivide(m_Value, Other.m_Value);
        return *this;
    }

    Vector& operator/=(float32_t Factor)
    {
        m_Value = DirectX::XMVectorScale(m_Value, 1.0f / Factor);
        return *this;
    }
protected:
    Vector(DirectX::XMVECTOR Value)
        : m_Value(Value)
    {
    }

    Vector(float32_t X, float32_t Y, float32_t Z, float32_t W)
    {
        DirectX::XMVectorSet(X, Y, Z, W);
    }

    Vector(uint32_t X, uint32_t Y, uint32_t Z, uint32_t W)
    {
        DirectX::XMVectorSetInt(X, Y, Z, W);
    }

    DirectX::XMVECTOR m_Value{};
};
#endif

class Vector2 : public Float2
{
public:
    inline Vector2()
        : Float2(0.0f, 0.0f)
    {
    }

    inline Vector2(float32_t Value)
        : Float2(Value, Value)
    {
    }

    inline Vector2(float32_t X, float32_t Y)
        : Float2(X, Y)
    {
    }

    inline Vector2(const float32_t* Array)
        : Float2(Array)
    {
    }

    inline Vector2& operator+=(const Vector2& Other) 
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorAdd(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&Other)));
    }
    inline Vector2& operator+=(float32_t Value)
    {
        Vector2 V(Value);
        DirectX::XMStoreFloat2(this, DirectX::XMVectorAdd(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&V)));
    }
    friend inline Vector2 operator+(const Vector2& Left, const Vector2& Right)
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorAdd(DirectX::XMLoadFloat2(&Left), DirectX::XMLoadFloat2(&Right)));
        return Ret;
    }
    friend inline Vector2 operator+(const Vector2& Left, float32_t Right)
    {
        Vector2 V(Right);
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorAdd(DirectX::XMLoadFloat2(&Left), DirectX::XMLoadFloat2(&V)));
        return Ret;
    }

    inline Vector2& operator-=(const Vector2& Other)
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&Other)));
    }
    inline Vector2& operator-=(float32_t Value)
    {
        Vector2 V(Value);
        DirectX::XMStoreFloat2(this, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&V)));
    }
    friend inline Vector2 operator-(const Vector2& Left, const Vector2& Right)
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&Left), DirectX::XMLoadFloat2(&Right)));
        return Ret;
    }
    inline friend Vector2 operator-(const Vector2& Left, float32_t Right)
    {
        Vector2 V(Right);
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&Left), DirectX::XMLoadFloat2(&V)));
        return Ret;
    }

    inline Vector2& operator*=(const float32_t Value)
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorScale(DirectX::XMLoadFloat2(this), Value));
    }
    inline Vector2& operator*=(const Vector2& Other)
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorMultiply(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&Other)));
    }
    inline friend Vector2 operator*(const Vector2& Left, const float32_t Right)
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

    inline Vector2& operator/=(const float32_t Value)
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorScale(DirectX::XMLoadFloat2(this), 1.0f / Value));
    }
    inline Vector2& operator/=(const Vector2& Other)
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorDivide(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&Other)));
    }
    inline friend Vector2 operator/(const Vector2& Left, const float32_t Right)
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

    float32_t Dot(const Vector2& Other) const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2Dot(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&Other)));
    }
    friend float32_t Dot(const Vector2& Left, const Vector2& Right)
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
    Vector2 Normalize() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(this)));
        return Ret;
    }

    void NormalizeEst()
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVector2NormalizeEst(DirectX::XMLoadFloat2(this)));
    }
    Vector2 NormalizeEst() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVector2NormalizeEst(DirectX::XMLoadFloat2(this)));
        return Ret;
    }

    void Negate()
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorNegate(DirectX::XMLoadFloat2(this)));
    }
    Vector2 Negate() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorNegate(DirectX::XMLoadFloat2(this)));
        return Ret;
    }

    float32_t LengthSq() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(DirectX::XMLoadFloat2(this)));
    }

    float32_t Length() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMLoadFloat2(this)));
    }

    float32_t LengthEst() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2LengthEst(DirectX::XMLoadFloat2(this)));
    }
    float32_t ReciprocalLength() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2ReciprocalLength(DirectX::XMLoadFloat2(this)));
    }

    float32_t ReciprocalLengthEst() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2ReciprocalLengthEst(DirectX::XMLoadFloat2(this)));
    }

    bool8_t IsNaN() const
    {
        return DirectX::XMVector4IsNaN(DirectX::XMLoadFloat2(this));
    }

    bool8_t IsInfinite() const
    {
        return DirectX::XMVector4IsInfinite(DirectX::XMLoadFloat2(this));
    }

    Vector2 Round() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorRound(DirectX::XMLoadFloat2(this)));
        return Ret;
    }
    void Round()
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorRound(DirectX::XMLoadFloat2(this)));
    }

    Vector2 Truncate() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorTruncate(DirectX::XMLoadFloat2(this)));
        return Ret;
    }
    void Truncate()
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorTruncate(DirectX::XMLoadFloat2(this)));
    }

    Vector2 Floor() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorFloor(DirectX::XMLoadFloat2(this)));
        return Ret;
    }
    void Floor()
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorFloor(DirectX::XMLoadFloat2(this)));
    }

    Vector2 Ceiling() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorCeiling(DirectX::XMLoadFloat2(this)));
        return Ret;
    }
    void Ceiling()
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorCeiling(DirectX::XMLoadFloat2(this)));
    }

    Vector2 Reciprocal() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorReciprocal(DirectX::XMLoadFloat2(this)));
        return Ret;
    }
    void Reciprocal()
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorReciprocal(DirectX::XMLoadFloat2(this)));
    }

    Vector2 ReciprocalEst() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorReciprocalEst(DirectX::XMLoadFloat2(this)));
        return Ret;
    }
    void ReciprocalEst()
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorReciprocalEst(DirectX::XMLoadFloat2(this)));
    }

    Vector2 Sqrt() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorSqrt(DirectX::XMLoadFloat2(this)));
        return Ret;
    }
    void Sqrt()
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorSqrt(DirectX::XMLoadFloat2(this)));
    }

    Vector2 SqrtEst() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorSqrtEst(DirectX::XMLoadFloat2(this)));
        return Ret;
    }
    void SqrtEst()
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorSqrtEst(DirectX::XMLoadFloat2(this)));
    }

    Vector2 Abs() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVectorAbs(DirectX::XMLoadFloat2(this)));
        return Ret;
    }
    void Abs()
    {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorAbs(DirectX::XMLoadFloat2(this)));
    }

    Vector2 Reflect(const Vector2& Normal) const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVector2Reflect(DirectX::XMLoadFloat2(this), DirectX::XMLoadFloat2(&Normal)));
        return Ret;
    }

    Vector2 Orthogonal() const
    {
        Vector2 Ret;
        DirectX::XMStoreFloat2(&Ret, DirectX::XMVector2Orthogonal(DirectX::XMLoadFloat2(this)));
        return Ret;
    }
};

#if 0
class Vector3 : public Vector2
{
public:
    using Vector2::Vector2;

    Vector3(const Vector& Other)
        : Vector2(Other)
    {
    }

    Vector3(float32_t X, float32_t Y, float32_t Z)
        : Vector2(X, Y, Z, 0.0f)
    {
    }

    Vector3(uint32_t X, uint32_t Y, uint32_t Z)
        : Vector2(X, Y, Z, 0u)
    {
    }

    float32_t Z() const
    {
        return DirectX::XMVectorGetZ(m_Value);
    }

    Vector3 Dot(const Vector3& Other) const
    {
        return DirectX::XMVector3Dot(m_Value, Other.m_Value);
    }

    friend Vector3 Dot(const Vector3& Left, const Vector3& Right)
    {
        return DirectX::XMVector3Dot(Left.m_Value, Right.m_Value);
    }

    Vector3 Cross(const Vector3& Other) const
    {
        return DirectX::XMVector3Cross(m_Value, Other.m_Value);
    }

    friend Vector3 Cross(const Vector3& Left, const Vector3& Right)
    {
        return DirectX::XMVector3Cross(Left.m_Value, Right.m_Value);
    }

    float32_t LengthSq() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(m_Value));
    }

    float32_t Length() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector3Length(m_Value));
    }

    float32_t LengthEst() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector3LengthEst(m_Value));
    }

    void Normalize()
    {
        m_Value = DirectX::XMVector3Normalize(m_Value);
    }

    void NormalizeEst()
    {
        m_Value = DirectX::XMVector3NormalizeEst(m_Value);
    }

    Vector3 Reflect(const Vector3& Normal) const
    {
        return DirectX::XMVector3Reflect(m_Value, Normal.m_Value);
    }

    Vector3 Orthogonal() const
    {
        return DirectX::XMVector3Orthogonal(m_Value);
    }
};

class Vector4 : public Vector3
{
public:
    using Vector3::Vector3;

    Vector4(const Vector& Other)
        : Vector3(Other)
    {
    }

    float32_t W() const
    {
        return DirectX::XMVectorGetW(m_Value);
    }

    Vector4 Dot(const Vector4& Other) const
    {
        return DirectX::XMVector4Dot(m_Value, Other.m_Value);
    }

    friend Vector4 Dot(const Vector4& Left, const Vector4& Right)
    {
        return DirectX::XMVector4Dot(Left.m_Value, Right.m_Value);
    }

    Vector4 Cross(const Vector4& V1, const Vector4& V2) const
    {
        return DirectX::XMVector4Cross(m_Value, V1.m_Value, V2.m_Value);
    }

    friend Vector4 Cross(const Vector4& V0, const Vector4& V1, const Vector4& V2)
    {
        return DirectX::XMVector4Cross(V0.m_Value, V1.m_Value, V2.m_Value);
    }

    float32_t LengthSq() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(m_Value));
    }

    float32_t Length() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector4Length(m_Value));
    }

    float32_t LengthEst() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector4LengthEst(m_Value));
    }

    void Normalize()
    {
        m_Value = DirectX::XMVector4Normalize(m_Value);
    }

    void NormalizeEst()
    {
        m_Value = DirectX::XMVector4NormalizeEst(m_Value);
    }

    Vector4 Reflect(const Vector4& Normal) const
    {
        return DirectX::XMVector4Reflect(m_Value, Normal.m_Value);
    }

    Vector4 Orthogonal() const
    {
        return DirectX::XMVector4Orthogonal(m_Value);
    }
};
#endif

NAMESPACE_END(Math)
