#ifndef ASTROVENTURE_STARTLAYER_H
#define ASTROVENTURE_STARTLAYER_H

#include "Layer.h"

#include "game/controller/StartController.h"

namespace UI {
class GameUIScene;

class StartLayer: public Layer {
public:
    explicit StartLayer(GameUIScene *scene);

    void updateValues();

protected:
    void onShow() override;

private:
    StartController m_controller;
};
} // UI

#endif //ASTROVENTURE_STARTLAYER_H
