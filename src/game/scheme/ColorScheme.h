#ifndef ASTROVENTURE_COLORSCHEME_H
#define ASTROVENTURE_COLORSCHEME_H

#include <algine/core/Color.h>
#include <tulz/static_initializer.h>
#include <functional>

using namespace algine;

class ColorScheme {
    STATIC_INITIALIZER_DECL

public:
    Color gameBackground {109, 61, 121};
    Color planetColor {88, 85, 176};
    Color crystalsColor {109, 61, 121};
    Color atmosphereColor {88, 85, 176};
    Color btnBackground {100, 83, 148, 127};
    Color btnPressedBackground {100, 83, 148, 217};

public:
    using Mixer = std::function<Color(const Color&, const Color&, float)>;

    static ColorScheme mix(const ColorScheme &src, const ColorScheme &dst, float pos, const Mixer &mixer);
};

#endif //ASTROVENTURE_COLORSCHEME_H
