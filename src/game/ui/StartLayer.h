#ifndef ASTROVENTURE_STARTLAYER_H
#define ASTROVENTURE_STARTLAYER_H

#include "Layer.h"

namespace UI {
class GameUIScene;

class StartLayer: public Layer {
public:
    explicit StartLayer(GameUIScene *scene);

    void updateValues();
};
} // UI

#endif //ASTROVENTURE_STARTLAYER_H
