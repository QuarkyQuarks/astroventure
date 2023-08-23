#ifndef ASTROVENTURE_MECHANICS_H
#define ASTROVENTURE_MECHANICS_H

#include "types.h"

#include <tulz/observer/Subject.h>

#include <vector>

using namespace tulz;

class Planet;
class GameScene;

namespace mechanics {
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
    bool isOnGround();
    bool isDestroyed();

    Subscription<> addOnLaunchListener(const Observer<> &listener);
    Subscription<Planet*> addOnLandingListener(const Observer<Planet*> &listener);
    Subscription<Planet*> addOnGroundListener(const Observer<Planet*> &listener);
    Subscription<> addOnDestroyedListener(const Observer<> &listener);

    void setBound(const vec2 &bound);

    void update();
    void launch();

private:
    Subject<> m_onLaunch;
    Subject<Planet*> m_onLanding;
    Subject<Planet*> m_onGround;
    Subject<> m_onDestroyed;

    State m_state;
    vec2 m_bound;

    GameScene &m_scene;

private:
    struct {
        std::vector<vec2> points;
        std::vector<vec2> velocity;
        Planet* landingPlanet;
        num_t flightStart;
    } m_trajectory;

private:
    void (Mechanics::*step_ptr) ();

    void takeOff();
    void trajectoryCalc();
    void flight();
    void motion(int id, num_t ratio);
    vec2 rotation(vec2 distToPlanet, num_t k) const;
    void orbitDocking();
    void landing();

    bool inBounds(const vec2 &pos) const;
};
}

#endif