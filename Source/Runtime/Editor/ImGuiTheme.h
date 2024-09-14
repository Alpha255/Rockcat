#pragma once

#include "Editor/ImGuiExtensions.h"
#include "Engine/Asset/SerializableAsset.h"

struct ImGuiTheme : public ImGuiStyle, public SerializableAsset<ImGuiTheme>
{
    template<class T>
    ImGuiTheme(T&& Path)
        : BaseClass::BaseClass(std::forward<T>(Path))
        , ImGuiStyle()
    {
    }

    std::string Name;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
            CEREAL_NVP(Alpha),
            CEREAL_NVP(DisabledAlpha),
            CEREAL_NVP(WindowPadding),
            CEREAL_NVP(WindowRounding),
            CEREAL_NVP(WindowBorderSize),
            CEREAL_NVP(WindowMinSize),
            CEREAL_NVP(WindowTitleAlign),
            CEREAL_NVP(WindowMenuButtonPosition),
            CEREAL_NVP(ChildRounding),
            CEREAL_NVP(ChildBorderSize),
            CEREAL_NVP(PopupRounding),
            CEREAL_NVP(PopupBorderSize),
            CEREAL_NVP(FramePadding),
            CEREAL_NVP(FrameRounding),
            CEREAL_NVP(FrameBorderSize),
            CEREAL_NVP(ItemSpacing),
            CEREAL_NVP(ItemInnerSpacing),
            CEREAL_NVP(CellPadding),
            CEREAL_NVP(TouchExtraPadding),
            CEREAL_NVP(IndentSpacing),
            CEREAL_NVP(ColumnsMinSpacing),
            CEREAL_NVP(ScrollbarSize),
            CEREAL_NVP(ScrollbarRounding),
            CEREAL_NVP(GrabMinSize),
            CEREAL_NVP(GrabRounding),
            CEREAL_NVP(LogSliderDeadzone),
            CEREAL_NVP(TabRounding),
            CEREAL_NVP(TabBorderSize),
            CEREAL_NVP(TabMinWidthForCloseButton),
            CEREAL_NVP(TabBarBorderSize),
            CEREAL_NVP(TabBarOverlineSize),
            CEREAL_NVP(TableAngledHeadersAngle),
            CEREAL_NVP(TableAngledHeadersTextAlign),
            CEREAL_NVP(ColorButtonPosition),
            CEREAL_NVP(ButtonTextAlign),
            CEREAL_NVP(SelectableTextAlign),
            CEREAL_NVP(SeparatorTextBorderSize),
            CEREAL_NVP(SeparatorTextAlign),
            CEREAL_NVP(SeparatorTextPadding),
            CEREAL_NVP(DisplayWindowPadding),
            CEREAL_NVP(DisplaySafeAreaPadding),
            CEREAL_NVP(DockingSeparatorSize),
            CEREAL_NVP(MouseCursorScale),
            CEREAL_NVP(AntiAliasedLines),
            CEREAL_NVP(AntiAliasedLinesUseTex),
            CEREAL_NVP(AntiAliasedFill),
            CEREAL_NVP(CurveTessellationTol),
            CEREAL_NVP(CircleTessellationMaxError),
            CEREAL_NVP(Colors),
            CEREAL_NVP(HoverStationaryDelay),
            CEREAL_NVP(HoverDelayShort),
            CEREAL_NVP(HoverDelayNormal),
            CEREAL_NVP(HoverFlagsForTooltipMouse),
            CEREAL_NVP(HoverFlagsForTooltipNav)
		);
	}
};