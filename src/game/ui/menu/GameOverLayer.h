#ifndef ASTROVENTURE_GAMEOVERLAYER_H
#define ASTROVENTURE_GAMEOVERLAYER_H

#include "MenuLayer.h"

namespace UI {
class GameOverLayer: public MenuLayer {
public:
    explicit GameOverLayer(GameUIScene *scene);
};
} // UI

#endif //ASTROVENTURE_GAMEOVERLAYER_H
