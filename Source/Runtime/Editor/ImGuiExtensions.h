#pragma once

#include "Core/Cereal.h"
#include "Core/Definitions.h"
#include "Submodules/imgui/imgui.h"

static inline ImVec2 operator*(const ImVec2& Left, const float Right)
{
    return ImVec2(Left.x * Right, Left.y * Right);
}
static inline ImVec2 operator/(const ImVec2& Left, const float Right)
{
    return ImVec2(Left.x / Right, Left.y / Right);
}
static inline ImVec2 operator+(const ImVec2& Left, const ImVec2& Right)
{
    return ImVec2(Left.x + Right.x, Left.y + Right.y);
}
static inline ImVec2 operator-(const ImVec2& Left, const ImVec2& Right)
{
    return ImVec2(Left.x - Right.x, Left.y - Right.y);
}
static inline ImVec2 operator*(const ImVec2& Left, const ImVec2& Right)
{
    return ImVec2(Left.x * Right.x, Left.y * Right.y);
}
static inline ImVec2 operator/(const ImVec2& Left, const ImVec2& Right)
{
    return ImVec2(Left.x / Right.x, Left.y / Right.y);
}
static inline ImVec2& operator+=(ImVec2& Left, const ImVec2& Right)
{
    Left.x += Right.x;
    Left.y += Right.y;
    return Left;
}
static inline ImVec2& operator-=(ImVec2& Left, const ImVec2& Right)
{
    Left.x -= Right.x;
    Left.y -= Right.y;
    return Left;
}
static inline ImVec2& operator*=(ImVec2& Left, const float Right)
{
    Left.x *= Right;
    Left.y *= Right;
    return Left;
}
static inline ImVec2& operator/=(ImVec2& Left, const float Right)
{
    Left.x /= Right;
    Left.y /= Right;
    return Left;
}
static inline ImVec4 operator-(const ImVec4& Left, const ImVec4& Right)
{
    return ImVec4(Left.x - Right.x, Left.y - Right.y, Left.z - Right.z, Left.w - Right.w);
}
static inline ImVec4 operator+(const ImVec4& Left, const ImVec4& Right)
{
    return ImVec4(Left.x + Right.x, Left.y + Right.y, Left.z + Right.z, Left.w + Right.w);
}
static inline ImVec4 operator*(const ImVec4& Left, const float Right)
{
    return ImVec4(Left.x * Right, Left.y * Right, Left.z * Right, Left.w * Right);
}
static inline ImVec4 operator/(const ImVec4& Left, const float Right)
{
    return ImVec4(Left.x / Right, Left.y / Right, Left.z / Right, Left.w / Right);
}
static inline ImVec4 operator*(const ImVec4& Left, const ImVec4& Right)
{
    return ImVec4(Left.x * Right.x, Left.y * Right.y, Left.z * Right.z, Left.w * Right.w);
}
static inline ImVec4 operator/(const ImVec4& Left, const ImVec4& Right)
{
    return ImVec4(Left.x / Right.x, Left.y / Right.y, Left.z / Right.z, Left.w / Right.w);
}
static inline ImVec4& operator+=(ImVec4& Left, const ImVec4& Right)
{
    Left.x += Right.x;
    Left.y += Right.y;
    Left.z += Right.z;
    Left.w += Right.w;
    return Left;
}
static inline ImVec4& operator-=(ImVec4& Left, const ImVec4& Right)
{
    Left.x -= Right.x;
    Left.y -= Right.y;
    Left.z -= Right.z;
    Left.w -= Right.w;
    return Left;
}
static inline ImVec4& operator*=(ImVec4& Left, const float Right)
{
    Left.x *= Right;
    Left.y *= Right;
    return Left;
}
static inline ImVec4& operator/=(ImVec4& Left, const float Right)
{
    Left.x /= Right;
    Left.y /= Right;
    return Left;
}

template<class Archive>
void serialize(Archive& Ar, ImVec2& Vec2)
{
    Ar(
        CEREAL_NVP(Vec2.x),
        CEREAL_NVP(Vec2.y)
    );
}

template<class Archive>
void serialize(Archive& Ar, ImVec4& Vec4)
{
    Ar(
        CEREAL_NVP(Vec4.x),
        CEREAL_NVP(Vec4.y),
        CEREAL_NVP(Vec4.z),
        CEREAL_NVP(Vec4.w)
    );
}

struct ImGuiFontConfigs : public ImFontConfig
{
    template<class Archive>
    void serialize(Archive& Ar)
    {
        Ar(
            CEREAL_NVP(FontNo),
            CEREAL_NVP(SizePixels),
            CEREAL_NVP(OversampleH),
            CEREAL_NVP(OversampleV),
            CEREAL_NVP(PixelSnapH),
            CEREAL_NVP(GlyphExtraSpacing),
            CEREAL_NVP(GlyphOffset),
            CEREAL_NVP(GlyphMinAdvanceX),
            CEREAL_NVP(GlyphMaxAdvanceX),
            CEREAL_NVP(MergeMode),
            CEREAL_NVP(FontBuilderFlags),
            CEREAL_NVP(RasterizerMultiply),
            CEREAL_NVP(RasterizerDensity),
            CEREAL_NVP(EllipsisChar),
            CEREAL_NVP(Name),
            CEREAL_NVP(FontSize),
            CEREAL_NVP(GlyphRangesPayload)
        );

        if (Archive::is_loading::value)
        {
            GlyphRanges = GlyphRangesPayload.data();
        }
    }

    float FontSize = 12.0f;
    std::vector<ImWchar> GlyphRangesPayload;
};
