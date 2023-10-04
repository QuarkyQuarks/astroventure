#include "Controller.h"
#include "game/GameScene.h"

Controller::Controller(GameScene &gameScene)
    : m_gameScene(gameScene) {}

void Controller::emitEventImpl(const Event &event) {
    m_gameScene.emitControllerEvent(event);
}