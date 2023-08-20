#ifndef ASTROVENTURE_COLORSCHEMEMANAGER_H
#define ASTROVENTURE_COLORSCHEMEMANAGER_H

#include <tulz/observer/Subject.h>
#include <tulz/static_initializer.h>

#include "ColorScheme.h"

using namespace tulz;

class GameScene;

class ColorSchemeManager {
    STATIC_INITIALIZER_DECL

public:
    explicit ColorSchemeManager(GameScene &scene);

    const ColorScheme& getColorScheme();

private:
    void update();

private:
    Subject<> m_onColorSchemeChanged;
    ColorScheme m_colorScheme;
};

#endif //ASTROVENTURE_COLORSCHEMEMANAGER_H
