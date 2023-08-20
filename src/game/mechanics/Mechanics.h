#ifndef ASTROVENTURE_MECHANICS_H
#define ASTROVENTURE_MECHANICS_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <tulz/observer/Subject.h>

#include <vector>

using namespace tulz;

class Planet;
class GameScene;

class Mechanics {
public:
    enum class State {
        Launch,
        Flight,
        Landing,
        Ground,
        Destroyed
    };

public:
    explicit Mechanics(GameScene &scene);

public:
    void launch();
    void update();

    bool isOnGround();
    bool isDestroyed();

    Subscription<> addOnLaunchListener(const Observer<> &listener);
    Subscription<Planet*> addOnLandingListener(const Observer<Planet*> &listener);
    Subscription<Planet*> addOnGroundListener(const Observer<Planet*> &listener);
    Subscription<> addOnDestroyedListener(const Observer<> &listener);

    void setBound(const glm::vec2 &bound);

private:
    Subject<> m_onLaunch;
    Subject<Planet*> m_onLanding;
    Subject<Planet*> m_onGround;
    Subject<> m_onDestroyed;

    State m_state;
    glm::vec2 m_bound;

    GameScene &m_scene;

private:
    // TODO: internal implementation here
};

#endif