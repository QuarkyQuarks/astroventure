#ifndef ASTROVENTURE_STARTCONTROLLER_H
#define ASTROVENTURE_STARTCONTROLLER_H

#include "Controller.h"

class StartController: public Controller {
public:
    enum class EventType {
        Start
    };

public:
    explicit StartController(GameScene &gameScene);

    void onActivated() override;
    void onDeactivated() override;

    void update() override;
    void event(const algine::Event &event) override;
};

#endif //ASTROVENTURE_STARTCONTROLLER_H
