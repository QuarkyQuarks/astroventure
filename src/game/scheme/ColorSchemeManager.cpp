#include "ColorSchemeManager.h"
#include "game/GameScene.h"

#include <algine/core/lua/Lua.h>

STATIC_INITIALIZER_IMPL(ColorSchemeManager) {
    Lua::getDefault().getModule("SEColorScheme").addLoader([](auto &args, sol::environment &env) {
        if (env["ColorSchemeManager"].valid())
            return;

        auto colorSchemeManager = env.new_usertype<ColorSchemeManager>("ColorSchemeManager");
        colorSchemeManager["getColorScheme"] = &ColorSchemeManager::getColorScheme;
    });
}

ColorSchemeManager::ColorSchemeManager(GameScene &scene) {
    // TODO: listen onScoreChanged
}

const ColorScheme& ColorSchemeManager::getColorScheme() {
    return m_colorScheme;
}

void ColorSchemeManager::update() {
    float step = 10.0f / 360.0f;
    m_colorScheme.rotate(step);
    m_onColorSchemeChanged.notify();
}
