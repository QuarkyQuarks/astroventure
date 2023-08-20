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
        colorScheme["btnBackground"] = &ColorScheme::btnBackground;
        colorScheme["btnPressedBackground"] = &ColorScheme::btnPressedBackground;
    });
}

void ColorScheme::rotate(float val) {
    gameBackground = gameBackground.lchHueRotateF(val);
    planetColor = planetColor.lchHueRotateF(val);
    crystalsColor = crystalsColor.lchHueRotateF(val);
    atmosphereColor = atmosphereColor.lchHueRotateF(val);
    btnBackground = btnBackground.lchHueRotateF(val);
    btnPressedBackground = btnPressedBackground.lchHueRotateF(val);
}
