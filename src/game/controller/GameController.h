#ifndef ASTROVENTURE_GAMECONTROLLER_H
#define ASTROVENTURE_GAMECONTROLLER_H

#include "Controller.h"

class GameController: public Controller {
public:
    explicit GameController(GameScene &gameScene);

    void update() override;
    void event(const algine::Event &event) override;
};

#endif //ASTROVENTURE_GAMECONTROLLER_H
