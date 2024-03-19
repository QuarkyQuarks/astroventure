#include "ColorScheme.h"

#include <algine/core/lua/Lua.h>

STATIC_INITIALIZER_IMPL(ColorScheme) {
    Lua::getDefault().getModule("SEColorScheme").addLoader([](auto &args, sol::environment &env) {
        if (env["ColorScheme"].valid())
            return;

        auto colorScheme = env.new_usertype<ColorScheme>("ColorScheme");
        colorScheme["gameBackground"] = &ColorScheme::gameBackground;
        colorScheme["planetColor"] = &ColorScheme::planetColor;
        colorScheme["crystalsColor"] = &ColorScheme::crystalsColor;
        colorScheme["atmosphereColor"] = &ColorScheme::atmosphereColor;
        colorScheme["btnBackground"] = &ColorScheme::btnBackground;
        colorScheme["btnPressedBackground"] = &ColorScheme::btnPressedBackground;
    });
}

ColorScheme ColorScheme::mix(const ColorScheme &src, const ColorScheme &dst, float pos, const Mixer &mixer) {
    auto mix = [&](const Color &src, const Color &dst) -> Color {
        return mixer(src, dst, pos);
    };

    return {
        .gameBackground = mix(src.gameBackground, dst.gameBackground),
        .planetColor = mix(src.planetColor, dst.planetColor),
        .crystalsColor = mix(src.crystalsColor, dst.crystalsColor),
        .atmosphereColor = mix(src.atmosphereColor, dst.atmosphereColor),
        .btnBackground = mix(src.btnBackground, dst.btnBackground),
        .btnPressedBackground = mix(src.btnPressedBackground, dst.btnPressedBackground)
    };
}
