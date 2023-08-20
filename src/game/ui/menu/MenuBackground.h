#ifndef ASTROVENTURE_MENUBACKGROUND_H
#define ASTROVENTURE_MENUBACKGROUND_H

#include <algine/std/Blur.h>

using namespace algine;

namespace UI {
class MenuBackground {
public:
    static void init(Texture2D *base, QuadRenderer *quadRenderer, int width, int height);

    static void update();

    static Texture2D* get();

    static void onSizeChanged(int width, int height);

    static int width(int width);
    static int height(int height);

private:
    static void calcMultiplier(int width, int height);

private:
    static Texture2D *m_base;
    static Blur *m_blur;
    static float m_multiplier;
};
} // UI

#endif //ASTROVENTURE_MENUBACKGROUND_H
