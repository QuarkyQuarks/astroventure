#ifndef ASTROVENTURE_PAUSELAYER_H
#define ASTROVENTURE_PAUSELAYER_H

#include "MenuLayer.h"

namespace UI {
class PauseLayer: public MenuLayer {
public:
    explicit PauseLayer(GameUIScene *scene);

protected:
    void onHide() override;
};
} // UI

#endif //ASTROVENTURE_PAUSELAYER_H
