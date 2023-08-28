#include "StateAction.h"
#include "game/GameScene.h"

#include <algine/core/Window.h>

StateAction::StateAction(GameScene &scene)
    : m_gameScene(scene) {}

void StateAction::trigger() {
    // This call is needed to prevent potential `onCompleted`
    // triggering while `onTrigger` event is being sent
    lock();

    m_onTrigger.notify();

    // Triggers `onCompleted` if needed
    unlock();
}

void StateAction::lock() {
    ++m_lockCount;
}

void StateAction::unlock() {
    if (--m_lockCount == 0) {
        m_gameScene.getParentWindow()->invokeLater([this] {
            m_onCompleted.notify();
        });
    }
}

Subscription<> StateAction::addOnTriggerListener(const Observer<> &listener) {
    return m_onTrigger.subscribe(listener);
}

Subscription<> StateAction::addOnCompletedListener(const Observer<> &listener) {
    return m_onCompleted.subscribe(listener);
}
