#pragma once

#include "Editor/ImGuiExtensions.h"

class ImGuiGizmo
{
public:
	enum class EMode
	{
		None,
		Selection = 1 << 0,
		TranslationX = 1 << 1,
		TranslationY = 1 << 2,
		TranslationZ = 1 << 3,
		Translation = TranslationX | TranslationY | TranslationZ,
		RotationX = 1 << 4,
		RotationY = 1 << 5,
		RotationZ = 1 << 6,
		Rotatation = RotationX | RotationY | RotationZ,
		ScalingX = 1 << 7,
		ScalingY = 1 << 8,
		ScalingZ = 1 << 9,
		Scaling = ScalingX | ScalingY | ScalingZ,
	};
};

