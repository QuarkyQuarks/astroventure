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

ColorSchemeManager::ColorSchemeManager(GameScene &scene)
    : m_keySchemes(keySchemes()),
      m_keyScheme(),
      m_colorScheme(m_keySchemes.front().colorScheme)
{
    scene.getScore().subscribe([this](int score) {
        update();
    });
}

const ColorScheme& ColorSchemeManager::getColorScheme() const {
    return m_colorScheme;
}

Subscription<> ColorSchemeManager::addOnChangedListener(const Observer<> &listener) {
    return m_onChanged.subscribe(listener);
}

void ColorSchemeManager::update() {
    constexpr float step = 0.1f;

    auto count = static_cast<int>(m_keySchemes.size());

    m_keyScheme = std::fmod(m_keyScheme + step, static_cast<float>(count));

    auto srcIndex = static_cast<int>(m_keyScheme);
    auto dstIndex = (srcIndex + 1) % count;
    auto pos = m_keyScheme - static_cast<float>(srcIndex);

    auto &srcSchemeDetails = m_keySchemes[srcIndex];
    auto &srcScheme = srcSchemeDetails.colorScheme;
    auto &dstScheme = m_keySchemes[dstIndex].colorScheme;
    auto &mixer = srcSchemeDetails.mixer;

    m_colorScheme = ColorScheme::mix(srcScheme, dstScheme, pos, mixer);

    m_onChanged.notify();
}

std::vector<ColorSchemeManager::ColorSchemeDetails> ColorSchemeManager::keySchemes() {
    return {
        {
            .colorScheme {
                .gameBackground {88, 85, 176},
                .planetColor {88, 85, 176},
                .crystalsColor {109, 61, 121},
                .atmosphereColor {88, 85, 176},
                .btnBackground {100, 83, 148, 127},
                .btnPressedBackground {100, 83, 148, 217}
            },
            .mixer = &Color::luvMix
        },
        {
            .colorScheme {
                .gameBackground {"#640002"},
                .planetColor {"#640002"},
                .crystalsColor {"#9C6431"},
                .atmosphereColor {"#640002"},
                .btnBackground {"#7F992E01"},
                .btnPressedBackground {"#D9992E01"}
            },
            .mixer = &Color::luvMix
        }
    };
}
