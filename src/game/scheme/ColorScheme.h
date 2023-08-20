#ifndef ASTROVENTURE_COLORSCHEME_H
#define ASTROVENTURE_COLORSCHEME_H

#include <algine/core/Color.h>

#include <tulz/static_initializer.h>

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
    void rotate(float val);
};

#endif //ASTROVENTURE_COLORSCHEME_H
