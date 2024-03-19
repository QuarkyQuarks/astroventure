#ifndef ASTROVENTURE_COLORSCHEMEMANAGER_H
#define ASTROVENTURE_COLORSCHEMEMANAGER_H

#include <tulz/observer/Subject.h>
#include <tulz/static_initializer.h>

#include "ColorScheme.h"

using namespace tulz;

class GameScene;

/**
 * The class that manages the game color scheme.
 * Color scheme changes when the score changes.
 */
class ColorSchemeManager {
    STATIC_INITIALIZER_DECL

public:
    explicit ColorSchemeManager(GameScene &scene);

    /**
     * @return The current color scheme.
     */
    const ColorScheme& getColorScheme() const;

    /**
     * Adds on color scheme changed listener
     * @param listener
     * @return
     */
    Subscription<> addOnChangedListener(const Observer<> &listener);

private:
    void update();

private:
    struct ColorSchemeDetails {
        /**
         * The current color scheme
         */
        ColorScheme colorScheme;

        /**
         * Will be used to mix the current
         * color scheme with the next
         */
        ColorScheme::Mixer mixer;
    };

    static std::vector<ColorSchemeDetails> keySchemes();

private:
    std::vector<ColorSchemeDetails> m_keySchemes;
    float m_keyScheme; // value in range [0, m_keySchemes.size())

private:
    Subject<> m_onChanged;
    ColorScheme m_colorScheme;
};

#endif //ASTROVENTURE_COLORSCHEMEMANAGER_H
