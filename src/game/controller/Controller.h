#ifndef ASTROVENTURE_CONTROLLER_H
#define ASTROVENTURE_CONTROLLER_H

#include <algine/core/unified/Event.h>

#include <glm/vec2.hpp>

class GameScene;

/**
 * Used to control behaviour for the different game
 * scenarios, "what must happen on non-gui click",
 * e.g. start screen, in game screen
 */
class Controller {
public:
    explicit Controller(GameScene &gameScene);
    virtual ~Controller() = default;

    virtual void update() = 0;
    virtual void event(const algine::Event &event) = 0;

protected:
    GameScene &m_gameScene;
};

#endif //ASTROVENTURE_CONTROLLER_H
