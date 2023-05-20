#pragma once

#include "Runtime/Core/Definitions.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>

NAMESPACE_START(Math)

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
        return DirectX::XMVector4GreaterEqual(m_Value, Other.m_Value);
    }

    bool8_t operator<(const Vector& Other) const
    {
        return DirectX::XMVector4Less(m_Value, Other.m_Value);
    }

    bool8_t operator<=(const Vector& Other) const
    {
        return DirectX::XMVector4LessEqual(m_Value, Other.m_Value);
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

class Vector2 : public Vector
{
public:
    using Vector::Vector;

    Vector2(const Vector& Other)
        : Vector(Other)
    {
    }

    Vector2(float32_t X, float32_t Y)
        : Vector(X, Y, 0.0f, 0.0f)
    {
    }

    Vector2(uint32_t X, uint32_t Y)
        : Vector(X, Y, 0u, 0u)
    {
    }

    float32_t X() const
    {
        return DirectX::XMVectorGetX(m_Value);
    }

    float32_t Y() const
    {
        return DirectX::XMVectorGetY(m_Value);
    }

    Vector2 Dot(const Vector2& Other) const
    {
        return DirectX::XMVector2Dot(m_Value, Other.m_Value);
    }

    friend Vector2 Dot(const Vector2& Left, const Vector2& Right)
    {
        return DirectX::XMVector2Dot(Left.m_Value, Right.m_Value);
    }

    Vector2 Cross(const Vector2& Other) const
    {
        return DirectX::XMVector2Cross(m_Value, Other.m_Value);
    }

    friend Vector2 Cross(const Vector2& Left, const Vector2& Right)
    {
        return DirectX::XMVector2Cross(Left.m_Value, Right.m_Value);
    }

    float32_t LengthSq() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(m_Value));
    }

    float32_t Length() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2Length(m_Value));
    }

    float32_t LengthEst() const
    {
        return DirectX::XMVectorGetX(DirectX::XMVector2LengthEst(m_Value));
    }

    void Normalize()
    {
        m_Value = DirectX::XMVector2Normalize(m_Value);
    }

    void NormalizeEst()
    {
        m_Value = DirectX::XMVector2NormalizeEst(m_Value);
    }

    Vector2 Reflect(const Vector2& Normal) const
    {
        return DirectX::XMVector2Reflect(m_Value, Normal.m_Value);
    }

    Vector2 Orthogonal() const
    {
        return DirectX::XMVector2Orthogonal(m_Value);
    }
};

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
        return DirectX::XMVectorGetX(DirectX::XMVectoreLengthEst(m_Value));
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

class Vector4 : public Vector
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

NAMESPACE_END(Math)
