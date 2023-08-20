#include "Mechanics.h"
#include "game/Cameraman.h"
#include "game/math/VecTools.h"

#include <algine/core/log/Log.h>
#include <glm/gtc/matrix_transform.hpp>

#define LOG_TAG "Mechanics"

Mechanics::Mechanics(GameScene &scene)
    : m_state(State::Ground),
      m_bound(),
      m_scene(scene)
{
    m_bound = mapScreenToWorld(
        glm::perspective(Cameraman::FieldOfView, 0.52f, Cameraman::Near, Cameraman::Far),
        {1.0f, 1.0f},
        -Cameraman::GamePos.z);
}

void Mechanics::launch() {
    Log::debug(LOG_TAG, __func__);
    // TODO: implement
}

void Mechanics::update() {
    // TODO: implement
}

bool Mechanics::isOnGround() {
    return m_state == State::Ground;
}

bool Mechanics::isDestroyed() {
    return m_state == State::Destroyed;
}

Subscription<> Mechanics::addOnLaunchListener(const Observer<> &listener) {
    return m_onLaunch.subscribe(listener);
}

Subscription<Planet*> Mechanics::addOnLandingListener(const Observer<Planet*> &listener) {
    return m_onLanding.subscribe(listener);
}

Subscription<Planet*> Mechanics::addOnGroundListener(const Observer<Planet*> &listener) {
    return m_onGround.subscribe(listener);
}

Subscription<> Mechanics::addOnDestroyedListener(const Observer<> &listener) {
    return m_onDestroyed.subscribe(listener);
}

void Mechanics::setBound(const glm::vec2 &bound) {
    m_bound = bound;
}
